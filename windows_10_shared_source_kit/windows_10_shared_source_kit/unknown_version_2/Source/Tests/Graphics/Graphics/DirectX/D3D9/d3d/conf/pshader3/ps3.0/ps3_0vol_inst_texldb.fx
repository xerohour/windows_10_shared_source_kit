#include "include.fx"

Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4, SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4, SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r0, v0
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r0, v0
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r0, v0
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c0, TexVol4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.0f, 0.0f, 0.0f, 0.f
            dcl_volume s4
			
			texldb r1, c0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c0, TexVol4, SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.0f, 0.0f, 0.48f, 0.f
            dcl_volume s4
			
			texldb r1, c0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c0, TexVol4, SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.0f, 0.0f, 0.98f, 0.f
            dcl_volume s4
			
			texldb r1, c0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4, writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1.xyzw, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; SliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1.xyzw, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; SliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1.xyzw, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r0, v0
			texldb r1.xyzw, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; SliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r0, v0
			texldb r1.xyzw, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; SliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r0, v0
			texldb r1.xyzw, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c0, TexVol4; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.98f, 0.98f, 0.0f, 0.f
            dcl_volume s4
			
			texldb r1.xyzw, c0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c0, TexVol4, SliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.98f, 0.98f, 0.48f, 0.f
            dcl_volume s4
			
			texldb r1.xyzw, c0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c0, TexVol4, SliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.98f, 0.98f, 0.98f, 0.f
            dcl_volume s4
			
			texldb r1.xyzw, c0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : v0, TexVol4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb_pp r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : v0, TexVol4, SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb_pp r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : v0, TexVol4, SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb_pp r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : r0, TexVol4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r0, v0
			texldb_pp r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : r0, TexVol4; SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r0, v0
			texldb_pp r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : r0, TexVol4; SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r0, v0
			texldb_pp r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : c0, TexVol4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.48f, 0.48f, 0.0f, 0.f
            dcl_volume s4
			
			texldb_pp r1, c0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : c0, TexVol4, SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.48f, 0.48f, 0.48f, 0.f
            dcl_volume s4
			
			texldb_pp r1, c0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : c0, TexVol4, SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.48f, 0.48f, 0.98f, 0.f
            dcl_volume s4
			
			texldb_pp r1, c0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; w = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; sliceX, w = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; sliceY, w = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol4; w = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s4
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol4; sliceX, w = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s4
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol4; sliceY, w = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s4
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol4; w = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s4
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol4; sliceX, w = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s4
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol4; sliceY, w = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s4
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol4; w = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s4
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol4; sliceX, w = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s4
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol4; sliceY, w = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s4
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol4; w = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s4
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol4; sliceX, w = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s4
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol4; sliceY, w = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s4
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol4; w = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s4
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol4; sliceX, w = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s4
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol4; sliceY, w = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s4
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol4; w = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s4
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol4; sliceX, w = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s4
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol4; sliceY, w = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s4
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; level = 5, w = -0";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; sliceX, level = 5, w = -0";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; sliceY, level = 5, w = -0";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol4; level = 5, w = -1";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s4
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol4; sliceX, level = 5, w = -1";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s4
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol4; sliceY, level = 5, w = -1";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s4
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol4; level = 5, w = -2";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s4
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol4; sliceX, level = 5, w = -2";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s4
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol4; sliceY, level = 5, w = -2";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s4
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol4; level = 5, w = -3";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s4
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol4; sliceX, level = 5, w = -3";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s4
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol4; sliceY, level = 5, w = -3";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s4
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol4; level = 5, w = -4";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -4.0, -4.0, -4.0, -4.0
            dcl_volume s4
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol4; sliceX, level = 5, w = -4";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -4.0, -4.0, -4.0, -4.0
            dcl_volume s4
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol4; sliceY, level = 5, w = -4";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -4.0, -4.0, -4.0, -4.0
            dcl_volume s4
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol4; level = 5, w = -5";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -5.0, -5.0, -5.0, -5.0
            dcl_volume s4
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol4; sliceX, level = 5, w = -5";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -5.0, -5.0, -5.0, -5.0
            dcl_volume s4
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol4; sliceY, level = 5, w = -5";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -5.0, -5.0, -5.0, -5.0
            dcl_volume s4
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol4; level = 5, w = -6";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -6.0, -6.0, -6.0, -6.0
            dcl_volume s4
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol4; sliceX, level = 5, w = -6";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -6.0, -6.0, -6.0, -6.0
            dcl_volume s4
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol4; sliceY, level = 5, w = -6";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_1>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -6.0, -6.0, -6.0, -6.0
            dcl_volume s4
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; w = 0, MaxMipLevel[4] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceX, w = 0, MaxMipLevel[4] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceY, w = 0, MaxMipLevel[4] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; w = 0, MaxMipLevel[4] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 1;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceX, w = 0, MaxMipLevel[4] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 1;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceY, w = 0, MaxMipLevel[4] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 1;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; w = 0, MaxMipLevel[4] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 2;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceX, w = 0, MaxMipLevel[4] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 2;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceY, w = 0, MaxMipLevel[4] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 2;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; w = 0, MaxMipLevel[4] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 3;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceX, w = 0, MaxMipLevel[4] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 3;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceY, w = 0, MaxMipLevel[4] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 3;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; w = 0, MaxMipLevel[4] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 4;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceX, w = 0, MaxMipLevel[4] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 4;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceY, w = 0, MaxMipLevel[4] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 4;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; w = 0, MaxMipLevel[4] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 5;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceX, w = 0, MaxMipLevel[4] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 5;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceY, w = 0, MaxMipLevel[4] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 5;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; w = 0, MaxMipLevel[4] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 6;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceX, w = 0, MaxMipLevel[4] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 6;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceY, w = 0, MaxMipLevel[4] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 6;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; w = 0, MaxMipLevel[4] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r0, v0
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; sliceX, w = 0, MaxMipLevel[4] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r0, v0
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; sliceY, w = 0, MaxMipLevel[4] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r0, v0
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol4; w = 0, MaxMipLevel[4] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 1;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r1, v0
			texldb r1, r1, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol4; sliceX, w = 0, MaxMipLevel[4] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 1;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r1, v0
			texldb r1, r1, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol4; sliceY, w = 0, MaxMipLevel[4] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 1;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r1, v0
			texldb r1, r1, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol4; w = 0, MaxMipLevel[4] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 2;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r2, v0
			texldb r1, r2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol4; sliceX, w = 0, MaxMipLevel[4] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 2;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r2, v0
			texldb r1, r2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol4; sliceY, w = 0, MaxMipLevel[4] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 2;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r2, v0
			texldb r1, r2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol4; w = 0, MaxMipLevel[4] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 3;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r3, v0
			texldb r1, r3, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol4; sliceX, w = 0, MaxMipLevel[4] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 3;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r3, v0
			texldb r1, r3, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol4; sliceY, w = 0, MaxMipLevel[4] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 3;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r3, v0
			texldb r1, r3, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol4; w = 0, MaxMipLevel[4] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 4;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r4, v0
			texldb r1, r4, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol4; sliceX, w = 0, MaxMipLevel[4] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 4;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r4, v0
			texldb r1, r4, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol4; sliceY, w = 0, MaxMipLevel[4] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 4;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r4, v0
			texldb r1, r4, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol4; w = 0, MaxMipLevel[4] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 5;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r5, v0
			texldb r1, r5, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol4; sliceX, w = 0, MaxMipLevel[4] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 5;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r5, v0
			texldb r1, r5, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol4; sliceY, w = 0, MaxMipLevel[4] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 5;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r5, v0
			texldb r1, r5, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol4; w = 0, MaxMipLevel[4] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 6;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r6, v0
			texldb r1, r6, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol4; sliceX, w = 0, MaxMipLevel[4] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 6;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r6, v0
			texldb r1, r6, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol4; sliceY, w = 0, MaxMipLevel[4] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 6;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r6, v0
			texldb r1, r6, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; w = 0, MipMapLODBias[4] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceX, w = 0, MipMapLODBias[4] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceY, w = 0, MipMapLODBias[4] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; w = 0, MipMapLODBias[4] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceX, w = 0, MipMapLODBias[4] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceY, w = 0, MipMapLODBias[4] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; w = 0, MipMapLODBias[4] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceX, w = 0, MipMapLODBias[4] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceY, w = 0, MipMapLODBias[4] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; w = 0, MipMapLODBias[4] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceX, w = 0, MipMapLODBias[4] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceY, w = 0, MipMapLODBias[4] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; w = 0, MipMapLODBias[4] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceX, w = 0, MipMapLODBias[4] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceY, w = 0, MipMapLODBias[4] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; w = 0, MipMapLODBias[4] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceX, w = 0, MipMapLODBias[4] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceY, w = 0, MipMapLODBias[4] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; w = 0, MipMapLODBias[4] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceX, w = 0, MipMapLODBias[4] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol4; sliceY, w = 0, MipMapLODBias[4] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			texldb r1, v0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; w = 0, MipMapLODBias[4] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r0, v0
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; sliceX, w = 0, MipMapLODBias[4] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r0, v0
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; sliceY, w = 0, MipMapLODBias[4] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r0, v0
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol4; w = 0, MipMapLODBias[4] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r1, v0
			texldb r1, r1, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol4; sliceX, w = 0, MipMapLODBias[4] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r1, v0
			texldb r1, r1, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol4; sliceY, w = 0, MipMapLODBias[4] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r1, v0
			texldb r1, r1, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol4; w = 0, MipMapLODBias[4] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r2, v0
			texldb r1, r2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol4; sliceX, w = 0, MipMapLODBias[4] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r2, v0
			texldb r1, r2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol4; sliceY, w = 0, MipMapLODBias[4] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r2, v0
			texldb r1, r2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol4; w = 0, MipMapLODBias[4] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r3, v0
			texldb r1, r3, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol4; sliceX, w = 0, MipMapLODBias[4] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r3, v0
			texldb r1, r3, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol4; sliceY, w = 0, MipMapLODBias[4] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r3, v0
			texldb r1, r3, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol4; w = 0, MipMapLODBias[4] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r4, v0
			texldb r1, r4, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol4; sliceX, w = 0, MipMapLODBias[4] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r4, v0
			texldb r1, r4, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol4; sliceY, w = 0, MipMapLODBias[4] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r4, v0
			texldb r1, r4, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol4; w = 0, MipMapLODBias[4] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r5, v0
			texldb r1, r5, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol4; sliceX, w = 0, MipMapLODBias[4] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r5, v0
			texldb r1, r5, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol4; sliceY, w = 0, MipMapLODBias[4] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r5, v0
			texldb r1, r5, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol4; w = 0, MipMapLODBias[4] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r6, v0
			texldb r1, r6, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol4; sliceX, w = 0, MipMapLODBias[4] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r6, v0
			texldb r1, r6, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol4; sliceY, w = 0, MipMapLODBias[4] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s4
			
			mov r6, v0
			texldb r1, r6, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; w = 5, MipMapLODBias[4] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; sliceX, w = 5, MipMapLODBias[4] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; sliceY, w = 5, MipMapLODBias[4] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; w = 5, MipMapLODBias[4] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; sliceX, w = 5, MipMapLODBias[4] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; sliceY, w = 5, MipMapLODBias[4] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; w = 5, MipMapLODBias[4] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; sliceX, w = 5, MipMapLODBias[4] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; sliceY, w = 5, MipMapLODBias[4] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; w = 5, MipMapLODBias[4] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; sliceX, w = 5, MipMapLODBias[4] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; sliceY, w = 5, MipMapLODBias[4] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; w = 5, MipMapLODBias[4] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; sliceX, w = 5, MipMapLODBias[4] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; sliceY, w = 5, MipMapLODBias[4] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; w = 5, MipMapLODBias[4] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; sliceX, w = 5, MipMapLODBias[4] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; sliceY, w = 5, MipMapLODBias[4] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; w = 5, MipMapLODBias[4] = -6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; sliceX, w = 5, MipMapLODBias[4] = -6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol4; sliceY, w = 5, MipMapLODBias[4] = -6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			texldb r1, v2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; w = 5, MipMapLODBias[4] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r0, v2
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; sliceX, w = 5, MipMapLODBias[4] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r0, v2
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol4; sliceY, w = 5, MipMapLODBias[4] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r0, v2
			texldb r1, r0, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol4; w = 5, MipMapLODBias[4] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r1, v2
			texldb r1, r1, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol4; sliceX, w = 5, MipMapLODBias[4] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r1, v2
			texldb r1, r1, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol4; sliceY, w = 5, MipMapLODBias[4] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r1, v2
			texldb r1, r1, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol4; w = 5, MipMapLODBias[4] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r2, v2
			texldb r1, r2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol4; sliceX, w = 5, MipMapLODBias[4] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r2, v2
			texldb r1, r2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol4; sliceY, w = 5, MipMapLODBias[4] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r2, v2
			texldb r1, r2, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol4; w = 5, MipMapLODBias[4] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r3, v2
			texldb r1, r3, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol4; sliceX, w = 5, MipMapLODBias[4] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r3, v2
			texldb r1, r3, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol4; sliceY, w = 5, MipMapLODBias[4] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r3, v2
			texldb r1, r3, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol4; w = 5, MipMapLODBias[4] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r4, v2
			texldb r1, r4, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol4; sliceX, w = 5, MipMapLODBias[4] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r4, v2
			texldb r1, r4, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol4; sliceY, w = 5, MipMapLODBias[4] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r4, v2
			texldb r1, r4, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol4; w = 5, MipMapLODBias[4] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r5, v2
			texldb r1, r5, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol4; sliceX, w = 5, MipMapLODBias[4] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r5, v2
			texldb r1, r5, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol4; sliceY, w = 5, MipMapLODBias[4] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r5, v2
			texldb r1, r5, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol4; w = 5, MipMapLODBias[4] = -6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r6, v2
			texldb r1, r6, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol4; sliceX, w = 5, MipMapLODBias[4] = -6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r6, v2
			texldb r1, r6, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol4; sliceY, w = 5, MipMapLODBias[4] = -6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s4
			
			mov r6, v2
			texldb r1, r6, s4
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v1, TexVol6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			texldb r1, v1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v1, TexVol6, SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			texldb r1, v1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v1, TexVol6, SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			texldb r1, v1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			mov r1, v1
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			mov r1, v1
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			mov r1, v1
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c1, TexVol6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.0f, 0.0f, 0.0f, 1.f
            dcl_volume s6
			
			texldb r1, c1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c1, TexVol6, SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.0f, 0.0f, 0.48f, 1.f
            dcl_volume s6
			
			texldb r1, c1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c1, TexVol6, SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.0f, 0.0f, 0.98f, 1.f
            dcl_volume s6
			
			texldb r1, c1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v1, TexVol6, writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			texldb r1.xyzw, v1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v1, TexVol6; SliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			texldb r1.xyzw, v1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v1, TexVol6; SliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			texldb r1.xyzw, v1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			mov r1, v1
			texldb r1.xyzw, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; SliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			mov r1, v1
			texldb r1.xyzw, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; SliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			mov r1, v1
			texldb r1.xyzw, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c1, TexVol6; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.98f, 0.98f, 0.0f, 1.f
            dcl_volume s6
			
			texldb r1.xyzw, c1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c1, TexVol6, SliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.98f, 0.98f, 0.48f, 1.f
            dcl_volume s6
			
			texldb r1.xyzw, c1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c1, TexVol6, SliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.98f, 0.98f, 0.98f, 1.f
            dcl_volume s6
			
			texldb r1.xyzw, c1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : v1, TexVol6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			texldb_pp r1, v1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : v1, TexVol6, SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			texldb_pp r1, v1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : v1, TexVol6, SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			texldb_pp r1, v1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : r1, TexVol6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			mov r1, v1
			texldb_pp r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : r1, TexVol6; SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			mov r1, v1
			texldb_pp r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : r1, TexVol6; SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s6
			
			mov r1, v1
			texldb_pp r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : c1, TexVol6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.48f, 0.48f, 0.0f, 1.f
            dcl_volume s6
			
			texldb_pp r1, c1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : c1, TexVol6, SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.48f, 0.48f, 0.48f, 1.f
            dcl_volume s6
			
			texldb_pp r1, c1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : c1, TexVol6, SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.48f, 0.48f, 0.98f, 1.f
            dcl_volume s6
			
			texldb_pp r1, c1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol6; w = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol6; sliceX, w = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol6; sliceY, w = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; w = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s6
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; sliceX, w = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s6
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; sliceY, w = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s6
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol6; w = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s6
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol6; sliceX, w = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s6
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol6; sliceY, w = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s6
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol6; w = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s6
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol6; sliceX, w = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s6
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol6; sliceY, w = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s6
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol6; w = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s6
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol6; sliceX, w = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s6
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol6; sliceY, w = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s6
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol6; w = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s6
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol6; sliceX, w = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s6
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol6; sliceY, w = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s6
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol6; w = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s6
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol6; sliceX, w = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s6
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol6; sliceY, w = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s6
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol6; level = 5, w = -0";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol6; sliceX, level = 5, w = -0";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol6; sliceY, level = 5, w = -0";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; level = 5, w = -1";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s6
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; sliceX, level = 5, w = -1";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s6
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; sliceY, level = 5, w = -1";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s6
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol6; level = 5, w = -2";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s6
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol6; sliceX, level = 5, w = -2";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s6
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol6; sliceY, level = 5, w = -2";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s6
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol6; level = 5, w = -3";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s6
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol6; sliceX, level = 5, w = -3";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s6
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol6; sliceY, level = 5, w = -3";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s6
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol6; level = 5, w = -4";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -4.0, -4.0, -4.0, -4.0
            dcl_volume s6
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol6; sliceX, level = 5, w = -4";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -4.0, -4.0, -4.0, -4.0
            dcl_volume s6
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol6; sliceY, level = 5, w = -4";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -4.0, -4.0, -4.0, -4.0
            dcl_volume s6
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol6; level = 5, w = -5";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -5.0, -5.0, -5.0, -5.0
            dcl_volume s6
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol6; sliceX, level = 5, w = -5";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -5.0, -5.0, -5.0, -5.0
            dcl_volume s6
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol6; sliceY, level = 5, w = -5";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -5.0, -5.0, -5.0, -5.0
            dcl_volume s6
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol6; level = 5, w = -6";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -6.0, -6.0, -6.0, -6.0
            dcl_volume s6
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol6; sliceX, level = 5, w = -6";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -6.0, -6.0, -6.0, -6.0
            dcl_volume s6
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol6; sliceY, level = 5, w = -6";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -6.0, -6.0, -6.0, -6.0
            dcl_volume s6
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; w = 0, MaxMipLevel[6] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceX, w = 0, MaxMipLevel[6] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceY, w = 0, MaxMipLevel[6] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; w = 0, MaxMipLevel[6] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 1;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceX, w = 0, MaxMipLevel[6] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 1;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceY, w = 0, MaxMipLevel[6] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 1;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; w = 0, MaxMipLevel[6] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceX, w = 0, MaxMipLevel[6] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceY, w = 0, MaxMipLevel[6] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; w = 0, MaxMipLevel[6] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 3;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceX, w = 0, MaxMipLevel[6] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 3;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceY, w = 0, MaxMipLevel[6] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 3;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; w = 0, MaxMipLevel[6] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceX, w = 0, MaxMipLevel[6] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceY, w = 0, MaxMipLevel[6] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; w = 0, MaxMipLevel[6] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 5;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceX, w = 0, MaxMipLevel[6] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 5;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceY, w = 0, MaxMipLevel[6] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 5;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; w = 0, MaxMipLevel[6] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 6;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceX, w = 0, MaxMipLevel[6] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 6;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceY, w = 0, MaxMipLevel[6] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 6;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol6; w = 0, MaxMipLevel[6] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r0, v0
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol6; sliceX, w = 0, MaxMipLevel[6] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r0, v0
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol6; sliceY, w = 0, MaxMipLevel[6] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r0, v0
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; w = 0, MaxMipLevel[6] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 1;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r1, v0
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; sliceX, w = 0, MaxMipLevel[6] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 1;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r1, v0
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; sliceY, w = 0, MaxMipLevel[6] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 1;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r1, v0
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol6; w = 0, MaxMipLevel[6] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r2, v0
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol6; sliceX, w = 0, MaxMipLevel[6] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r2, v0
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol6; sliceY, w = 0, MaxMipLevel[6] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r2, v0
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol6; w = 0, MaxMipLevel[6] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 3;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r3, v0
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol6; sliceX, w = 0, MaxMipLevel[6] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 3;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r3, v0
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol6; sliceY, w = 0, MaxMipLevel[6] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 3;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r3, v0
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol6; w = 0, MaxMipLevel[6] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r4, v0
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol6; sliceX, w = 0, MaxMipLevel[6] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r4, v0
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol6; sliceY, w = 0, MaxMipLevel[6] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r4, v0
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol6; w = 0, MaxMipLevel[6] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 5;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r5, v0
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol6; sliceX, w = 0, MaxMipLevel[6] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 5;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r5, v0
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol6; sliceY, w = 0, MaxMipLevel[6] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 5;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r5, v0
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol6; w = 0, MaxMipLevel[6] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 6;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r6, v0
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol6; sliceX, w = 0, MaxMipLevel[6] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 6;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r6, v0
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol6; sliceY, w = 0, MaxMipLevel[6] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 6;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r6, v0
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; w = 0, MipMapLODBias[6] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceX, w = 0, MipMapLODBias[6] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceY, w = 0, MipMapLODBias[6] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; w = 0, MipMapLODBias[6] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceX, w = 0, MipMapLODBias[6] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceY, w = 0, MipMapLODBias[6] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; w = 0, MipMapLODBias[6] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceX, w = 0, MipMapLODBias[6] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceY, w = 0, MipMapLODBias[6] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; w = 0, MipMapLODBias[6] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceX, w = 0, MipMapLODBias[6] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceY, w = 0, MipMapLODBias[6] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; w = 0, MipMapLODBias[6] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceX, w = 0, MipMapLODBias[6] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceY, w = 0, MipMapLODBias[6] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; w = 0, MipMapLODBias[6] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceX, w = 0, MipMapLODBias[6] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceY, w = 0, MipMapLODBias[6] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; w = 0, MipMapLODBias[6] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceX, w = 0, MipMapLODBias[6] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol6; sliceY, w = 0, MipMapLODBias[6] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			texldb r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol6; w = 0, MipMapLODBias[6] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r0, v0
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol6; sliceX, w = 0, MipMapLODBias[6] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r0, v0
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol6; sliceY, w = 0, MipMapLODBias[6] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r0, v0
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; w = 0, MipMapLODBias[6] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r1, v0
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; sliceX, w = 0, MipMapLODBias[6] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r1, v0
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; sliceY, w = 0, MipMapLODBias[6] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r1, v0
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol6; w = 0, MipMapLODBias[6] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r2, v0
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol6; sliceX, w = 0, MipMapLODBias[6] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r2, v0
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol6; sliceY, w = 0, MipMapLODBias[6] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r2, v0
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol6; w = 0, MipMapLODBias[6] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r3, v0
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol6; sliceX, w = 0, MipMapLODBias[6] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r3, v0
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol6; sliceY, w = 0, MipMapLODBias[6] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r3, v0
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol6; w = 0, MipMapLODBias[6] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r4, v0
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol6; sliceX, w = 0, MipMapLODBias[6] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r4, v0
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol6; sliceY, w = 0, MipMapLODBias[6] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r4, v0
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol6; w = 0, MipMapLODBias[6] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r5, v0
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol6; sliceX, w = 0, MipMapLODBias[6] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r5, v0
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol6; sliceY, w = 0, MipMapLODBias[6] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r5, v0
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol6; w = 0, MipMapLODBias[6] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r6, v0
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol6; sliceX, w = 0, MipMapLODBias[6] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r6, v0
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol6; sliceY, w = 0, MipMapLODBias[6] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s6
			
			mov r6, v0
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; w = 5, MipMapLODBias[6] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; sliceX, w = 5, MipMapLODBias[6] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; sliceY, w = 5, MipMapLODBias[6] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; w = 5, MipMapLODBias[6] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; sliceX, w = 5, MipMapLODBias[6] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; sliceY, w = 5, MipMapLODBias[6] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; w = 5, MipMapLODBias[6] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; sliceX, w = 5, MipMapLODBias[6] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; sliceY, w = 5, MipMapLODBias[6] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; w = 5, MipMapLODBias[6] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; sliceX, w = 5, MipMapLODBias[6] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; sliceY, w = 5, MipMapLODBias[6] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; w = 5, MipMapLODBias[6] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; sliceX, w = 5, MipMapLODBias[6] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; sliceY, w = 5, MipMapLODBias[6] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; w = 5, MipMapLODBias[6] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; sliceX, w = 5, MipMapLODBias[6] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; sliceY, w = 5, MipMapLODBias[6] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; w = 5, MipMapLODBias[6] = -6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; sliceX, w = 5, MipMapLODBias[6] = -6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol6; sliceY, w = 5, MipMapLODBias[6] = -6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			texldb r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol6; w = 5, MipMapLODBias[6] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r0, v2
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol6; sliceX, w = 5, MipMapLODBias[6] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r0, v2
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol6; sliceY, w = 5, MipMapLODBias[6] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r0, v2
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; w = 5, MipMapLODBias[6] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r1, v2
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; sliceX, w = 5, MipMapLODBias[6] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r1, v2
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol6; sliceY, w = 5, MipMapLODBias[6] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r1, v2
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol6; w = 5, MipMapLODBias[6] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r2, v2
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol6; sliceX, w = 5, MipMapLODBias[6] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r2, v2
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol6; sliceY, w = 5, MipMapLODBias[6] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r2, v2
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol6; w = 5, MipMapLODBias[6] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r3, v2
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol6; sliceX, w = 5, MipMapLODBias[6] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r3, v2
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol6; sliceY, w = 5, MipMapLODBias[6] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r3, v2
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol6; w = 5, MipMapLODBias[6] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r4, v2
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol6; sliceX, w = 5, MipMapLODBias[6] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r4, v2
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol6; sliceY, w = 5, MipMapLODBias[6] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r4, v2
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol6; w = 5, MipMapLODBias[6] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r5, v2
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol6; sliceX, w = 5, MipMapLODBias[6] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r5, v2
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol6; sliceY, w = 5, MipMapLODBias[6] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r5, v2
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol6; w = 5, MipMapLODBias[6] = -6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r6, v2
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol6; sliceX, w = 5, MipMapLODBias[6] = -6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r6, v2
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol6; sliceY, w = 5, MipMapLODBias[6] = -6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s6
			
			mov r6, v2
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11, SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11, SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r2, v2
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r2, v2
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r2, v2
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c2, TexVol11";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.0f, 0.0f, 0.0f, 2.f
            dcl_volume s11
			
			texldb r1, c2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c2, TexVol11, SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.0f, 0.0f, 0.48f, 2.f
            dcl_volume s11
			
			texldb r1, c2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c2, TexVol11, SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.0f, 0.0f, 0.98f, 2.f
            dcl_volume s11
			
			texldb r1, c2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11, writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1.xyzw, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; SliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1.xyzw, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; SliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1.xyzw, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r2, v2
			texldb r1.xyzw, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; SliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r2, v2
			texldb r1.xyzw, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; SliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r2, v2
			texldb r1.xyzw, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c2, TexVol11; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.98f, 0.98f, 0.0f, 2.f
            dcl_volume s11
			
			texldb r1.xyzw, c2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c2, TexVol11, SliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.98f, 0.98f, 0.48f, 2.f
            dcl_volume s11
			
			texldb r1.xyzw, c2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c2, TexVol11, SliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.98f, 0.98f, 0.98f, 2.f
            dcl_volume s11
			
			texldb r1.xyzw, c2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : v2, TexVol11";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb_pp r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : v2, TexVol11, SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb_pp r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : v2, TexVol11, SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb_pp r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : r2, TexVol11";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r2, v2
			texldb_pp r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : r2, TexVol11; SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r2, v2
			texldb_pp r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : r2, TexVol11; SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r2, v2
			texldb_pp r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : c2, TexVol11";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.48f, 0.48f, 0.0f, 2.f
            dcl_volume s11
			
			texldb_pp r1, c2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : c2, TexVol11, SliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.48f, 0.48f, 0.48f, 2.f
            dcl_volume s11
			
			texldb_pp r1, c2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : c2, TexVol11, SliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.48f, 0.48f, 0.98f, 2.f
            dcl_volume s11
			
			texldb_pp r1, c2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol11; w = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol11; sliceX, w = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol11; sliceY, w = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol11; w = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s11
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol11; sliceX, w = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s11
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol11; sliceY, w = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s11
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; w = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s11
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; sliceX, w = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s11
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; sliceY, w = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s11
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol11; w = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s11
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol11; sliceX, w = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s11
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol11; sliceY, w = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s11
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol11; w = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s11
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol11; sliceX, w = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s11
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol11; sliceY, w = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s11
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol11; w = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s11
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol11; sliceX, w = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s11
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol11; sliceY, w = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s11
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol11; w = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s11
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol11; sliceX, w = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s11
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol11; sliceY, w = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s11
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol11; level = 5, w = -0";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol11; sliceX, level = 5, w = -0";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol11; sliceY, level = 5, w = -0";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol11; level = 5, w = -1";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s11
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol11; sliceX, level = 5, w = -1";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s11
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol11; sliceY, level = 5, w = -1";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s11
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; level = 5, w = -2";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s11
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; sliceX, level = 5, w = -2";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s11
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; sliceY, level = 5, w = -2";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s11
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol11; level = 5, w = -3";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s11
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol11; sliceX, level = 5, w = -3";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s11
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol11; sliceY, level = 5, w = -3";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s11
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol11; level = 5, w = -4";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -4.0, -4.0, -4.0, -4.0
            dcl_volume s11
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol11; sliceX, level = 5, w = -4";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -4.0, -4.0, -4.0, -4.0
            dcl_volume s11
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol11; sliceY, level = 5, w = -4";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -4.0, -4.0, -4.0, -4.0
            dcl_volume s11
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol11; level = 5, w = -5";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -5.0, -5.0, -5.0, -5.0
            dcl_volume s11
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol11; sliceX, level = 5, w = -5";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -5.0, -5.0, -5.0, -5.0
            dcl_volume s11
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol11; sliceY, level = 5, w = -5";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -5.0, -5.0, -5.0, -5.0
            dcl_volume s11
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol11; level = 5, w = -6";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -6.0, -6.0, -6.0, -6.0
            dcl_volume s11
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol11; sliceX, level = 5, w = -6";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -6.0, -6.0, -6.0, -6.0
            dcl_volume s11
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol11; sliceY, level = 5, w = -6";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -6.0, -6.0, -6.0, -6.0
            dcl_volume s11
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; w = 0, MaxMipLevel[11] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceX, w = 0, MaxMipLevel[11] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceY, w = 0, MaxMipLevel[11] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; w = 0, MaxMipLevel[11] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceX, w = 0, MaxMipLevel[11] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceY, w = 0, MaxMipLevel[11] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; w = 0, MaxMipLevel[11] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 2;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceX, w = 0, MaxMipLevel[11] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 2;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceY, w = 0, MaxMipLevel[11] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 2;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; w = 0, MaxMipLevel[11] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceX, w = 0, MaxMipLevel[11] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceY, w = 0, MaxMipLevel[11] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; w = 0, MaxMipLevel[11] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 4;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceX, w = 0, MaxMipLevel[11] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 4;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceY, w = 0, MaxMipLevel[11] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 4;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; w = 0, MaxMipLevel[11] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceX, w = 0, MaxMipLevel[11] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceY, w = 0, MaxMipLevel[11] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; w = 0, MaxMipLevel[11] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 6;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceX, w = 0, MaxMipLevel[11] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 6;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceY, w = 0, MaxMipLevel[11] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 6;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol11; w = 0, MaxMipLevel[11] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r0, v0
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol11; sliceX, w = 0, MaxMipLevel[11] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r0, v0
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol11; sliceY, w = 0, MaxMipLevel[11] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r0, v0
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol11; w = 0, MaxMipLevel[11] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r1, v0
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol11; sliceX, w = 0, MaxMipLevel[11] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r1, v0
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol11; sliceY, w = 0, MaxMipLevel[11] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r1, v0
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; w = 0, MaxMipLevel[11] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 2;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r2, v0
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; sliceX, w = 0, MaxMipLevel[11] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 2;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r2, v0
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; sliceY, w = 0, MaxMipLevel[11] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 2;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r2, v0
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol11; w = 0, MaxMipLevel[11] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r3, v0
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol11; sliceX, w = 0, MaxMipLevel[11] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r3, v0
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol11; sliceY, w = 0, MaxMipLevel[11] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r3, v0
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol11; w = 0, MaxMipLevel[11] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 4;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r4, v0
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol11; sliceX, w = 0, MaxMipLevel[11] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 4;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r4, v0
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol11; sliceY, w = 0, MaxMipLevel[11] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 4;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r4, v0
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol11; w = 0, MaxMipLevel[11] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r5, v0
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol11; sliceX, w = 0, MaxMipLevel[11] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r5, v0
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol11; sliceY, w = 0, MaxMipLevel[11] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r5, v0
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol11; w = 0, MaxMipLevel[11] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 6;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r6, v0
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol11; sliceX, w = 0, MaxMipLevel[11] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 6;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r6, v0
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol11; sliceY, w = 0, MaxMipLevel[11] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 6;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r6, v0
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; w = 0, MipMapLODBias[11] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceX, w = 0, MipMapLODBias[11] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceY, w = 0, MipMapLODBias[11] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; w = 0, MipMapLODBias[11] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceX, w = 0, MipMapLODBias[11] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceY, w = 0, MipMapLODBias[11] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; w = 0, MipMapLODBias[11] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceX, w = 0, MipMapLODBias[11] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceY, w = 0, MipMapLODBias[11] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; w = 0, MipMapLODBias[11] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceX, w = 0, MipMapLODBias[11] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceY, w = 0, MipMapLODBias[11] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; w = 0, MipMapLODBias[11] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceX, w = 0, MipMapLODBias[11] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceY, w = 0, MipMapLODBias[11] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; w = 0, MipMapLODBias[11] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceX, w = 0, MipMapLODBias[11] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceY, w = 0, MipMapLODBias[11] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; w = 0, MipMapLODBias[11] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceX, w = 0, MipMapLODBias[11] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexVol11; sliceY, w = 0, MipMapLODBias[11] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			texldb r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol11; w = 0, MipMapLODBias[11] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r0, v0
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol11; sliceX, w = 0, MipMapLODBias[11] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r0, v0
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol11; sliceY, w = 0, MipMapLODBias[11] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r0, v0
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol11; w = 0, MipMapLODBias[11] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r1, v0
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol11; sliceX, w = 0, MipMapLODBias[11] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r1, v0
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol11; sliceY, w = 0, MipMapLODBias[11] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r1, v0
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; w = 0, MipMapLODBias[11] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r2, v0
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; sliceX, w = 0, MipMapLODBias[11] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r2, v0
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; sliceY, w = 0, MipMapLODBias[11] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r2, v0
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol11; w = 0, MipMapLODBias[11] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r3, v0
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol11; sliceX, w = 0, MipMapLODBias[11] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r3, v0
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol11; sliceY, w = 0, MipMapLODBias[11] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r3, v0
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol11; w = 0, MipMapLODBias[11] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r4, v0
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol11; sliceX, w = 0, MipMapLODBias[11] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r4, v0
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol11; sliceY, w = 0, MipMapLODBias[11] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r4, v0
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol11; w = 0, MipMapLODBias[11] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r5, v0
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol11; sliceX, w = 0, MipMapLODBias[11] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r5, v0
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol11; sliceY, w = 0, MipMapLODBias[11] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r5, v0
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol11; w = 0, MipMapLODBias[11] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r6, v0
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol11; sliceX, w = 0, MipMapLODBias[11] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r6, v0
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol11; sliceY, w = 0, MipMapLODBias[11] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s11
			
			mov r6, v0
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; w = 5, MipMapLODBias[11] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; sliceX, w = 5, MipMapLODBias[11] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; sliceY, w = 5, MipMapLODBias[11] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; w = 5, MipMapLODBias[11] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; sliceX, w = 5, MipMapLODBias[11] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; sliceY, w = 5, MipMapLODBias[11] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; w = 5, MipMapLODBias[11] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; sliceX, w = 5, MipMapLODBias[11] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; sliceY, w = 5, MipMapLODBias[11] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; w = 5, MipMapLODBias[11] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; sliceX, w = 5, MipMapLODBias[11] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; sliceY, w = 5, MipMapLODBias[11] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; w = 5, MipMapLODBias[11] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; sliceX, w = 5, MipMapLODBias[11] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; sliceY, w = 5, MipMapLODBias[11] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; w = 5, MipMapLODBias[11] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; sliceX, w = 5, MipMapLODBias[11] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; sliceY, w = 5, MipMapLODBias[11] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; w = 5, MipMapLODBias[11] = -6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; sliceX, w = 5, MipMapLODBias[11] = -6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexVol11; sliceY, w = 5, MipMapLODBias[11] = -6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			texldb r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol11; w = 5, MipMapLODBias[11] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r0, v2
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol11; sliceX, w = 5, MipMapLODBias[11] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r0, v2
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexVol11; sliceY, w = 5, MipMapLODBias[11] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r0, v2
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol11; w = 5, MipMapLODBias[11] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r1, v2
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol11; sliceX, w = 5, MipMapLODBias[11] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r1, v2
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexVol11; sliceY, w = 5, MipMapLODBias[11] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r1, v2
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; w = 5, MipMapLODBias[11] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r2, v2
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; sliceX, w = 5, MipMapLODBias[11] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r2, v2
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexVol11; sliceY, w = 5, MipMapLODBias[11] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r2, v2
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol11; w = 5, MipMapLODBias[11] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r3, v2
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol11; sliceX, w = 5, MipMapLODBias[11] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r3, v2
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexVol11; sliceY, w = 5, MipMapLODBias[11] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r3, v2
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol11; w = 5, MipMapLODBias[11] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r4, v2
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol11; sliceX, w = 5, MipMapLODBias[11] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r4, v2
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexVol11; sliceY, w = 5, MipMapLODBias[11] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r4, v2
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol11; w = 5, MipMapLODBias[11] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r5, v2
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol11; sliceX, w = 5, MipMapLODBias[11] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r5, v2
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexVol11; sliceY, w = 5, MipMapLODBias[11] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r5, v2
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol11; w = 5, MipMapLODBias[11] = -6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r6, v2
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol11; sliceX, w = 5, MipMapLODBias[11] = -6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r6, v2
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexVol11; sliceY, w = 5, MipMapLODBias[11] = -6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s11
			
			mov r6, v2
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
