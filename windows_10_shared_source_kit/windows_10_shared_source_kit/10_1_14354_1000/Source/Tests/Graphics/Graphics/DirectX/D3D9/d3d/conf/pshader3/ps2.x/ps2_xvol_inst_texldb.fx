#include "include.fx"

Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2, SliceX";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2, SliceY";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r0, t0
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; SliceX";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r0, t0
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; SliceY";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r0, t0
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2, writemask=xyzw";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1.xyzw, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; SliceX, writemask=xyzw";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1.xyzw, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; SliceY, writemask=xyzw";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1.xyzw, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; writemask=xyzw";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r0, t0
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; SliceX, writemask=xyzw";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r0, t0
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; SliceY, writemask=xyzw";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r0, t0
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : t0, TexVol2";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb_pp r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : t0, TexVol2, SliceX";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb_pp r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : t0, TexVol2, SliceY";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb_pp r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : r0, TexVol2";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r0, t0
			texldb_pp r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : r0, TexVol2; SliceX";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r0, t0
			texldb_pp r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : r0, TexVol2; SliceY";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r0, t0
			texldb_pp r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; w = 0";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; sliceX, w = 0";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; sliceY, w = 0";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol2; w = 1";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 1.0
            dcl_volume s2
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol2; sliceX, w = 1";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 1.0
            dcl_volume s2
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol2; sliceY, w = 1";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 1.0
            dcl_volume s2
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol2; w = 2";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 2.0
            dcl_volume s2
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol2; sliceX, w = 2";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 2.0
            dcl_volume s2
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol2; sliceY, w = 2";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 2.0
            dcl_volume s2
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol2; w = 3";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 3.0
            dcl_volume s2
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol2; sliceX, w = 3";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 3.0
            dcl_volume s2
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol2; sliceY, w = 3";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 3.0
            dcl_volume s2
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; level = 5, w = -0";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -0.0
            dcl_volume s2
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; sliceX, level = 5, w = -0";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -0.0
            dcl_volume s2
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; sliceY, level = 5, w = -0";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -0.0
            dcl_volume s2
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol2; level = 5, w = -1";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -1.0
            dcl_volume s2
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol2; sliceX, level = 5, w = -1";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -1.0
            dcl_volume s2
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol2; sliceY, level = 5, w = -1";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -1.0
            dcl_volume s2
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol2; level = 5, w = -2";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -2.0
            dcl_volume s2
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol2; sliceX, level = 5, w = -2";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -2.0
            dcl_volume s2
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol2; sliceY, level = 5, w = -2";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -2.0
            dcl_volume s2
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol2; level = 5, w = -3";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -3.0
            dcl_volume s2
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol2; sliceX, level = 5, w = -3";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -3.0
            dcl_volume s2
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol2; sliceY, level = 5, w = -3";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -3.0
            dcl_volume s2
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; w = 0, MaxMipLevel[2] = 0";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceX, w = 0, MaxMipLevel[2] = 0";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceY, w = 0, MaxMipLevel[2] = 0";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; w = 0, MaxMipLevel[2] = 1";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceX, w = 0, MaxMipLevel[2] = 1";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceY, w = 0, MaxMipLevel[2] = 1";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; w = 0, MaxMipLevel[2] = 2";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceX, w = 0, MaxMipLevel[2] = 2";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceY, w = 0, MaxMipLevel[2] = 2";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; w = 0, MaxMipLevel[2] = 3";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceX, w = 0, MaxMipLevel[2] = 3";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceY, w = 0, MaxMipLevel[2] = 3";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; w = 0, MaxMipLevel[2] = 4";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceX, w = 0, MaxMipLevel[2] = 4";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceY, w = 0, MaxMipLevel[2] = 4";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; w = 0, MaxMipLevel[2] = 5";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceX, w = 0, MaxMipLevel[2] = 5";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceY, w = 0, MaxMipLevel[2] = 5";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; w = 0, MaxMipLevel[2] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 6;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceX, w = 0, MaxMipLevel[2] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 6;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceY, w = 0, MaxMipLevel[2] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 6;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; w = 0, MaxMipLevel[2] = 0";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r0, t0
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; sliceX, w = 0, MaxMipLevel[2] = 0";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r0, t0
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; sliceY, w = 0, MaxMipLevel[2] = 0";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r0, t0
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol2; w = 0, MaxMipLevel[2] = 1";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r1, t0
			texldb r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol2; sliceX, w = 0, MaxMipLevel[2] = 1";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r1, t0
			texldb r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol2; sliceY, w = 0, MaxMipLevel[2] = 1";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r1, t0
			texldb r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol2; w = 0, MaxMipLevel[2] = 2";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r2, t0
			texldb r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol2; sliceX, w = 0, MaxMipLevel[2] = 2";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r2, t0
			texldb r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol2; sliceY, w = 0, MaxMipLevel[2] = 2";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r2, t0
			texldb r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol2; w = 0, MaxMipLevel[2] = 3";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r3, t0
			texldb r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol2; sliceX, w = 0, MaxMipLevel[2] = 3";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r3, t0
			texldb r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol2; sliceY, w = 0, MaxMipLevel[2] = 3";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r3, t0
			texldb r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol2; w = 0, MaxMipLevel[2] = 4";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r4, t0
			texldb r1, r4, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol2; sliceX, w = 0, MaxMipLevel[2] = 4";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r4, t0
			texldb r1, r4, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol2; sliceY, w = 0, MaxMipLevel[2] = 4";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r4, t0
			texldb r1, r4, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol2; w = 0, MaxMipLevel[2] = 5";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r5, t0
			texldb r1, r5, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol2; sliceX, w = 0, MaxMipLevel[2] = 5";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r5, t0
			texldb r1, r5, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol2; sliceY, w = 0, MaxMipLevel[2] = 5";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r5, t0
			texldb r1, r5, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol2; w = 0, MaxMipLevel[2] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 6;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r6, t0
			texldb r1, r6, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol2; sliceX, w = 0, MaxMipLevel[2] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 6;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r6, t0
			texldb r1, r6, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol2; sliceY, w = 0, MaxMipLevel[2] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 6;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r6, t0
			texldb r1, r6, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; w = 0, MipMapLODBias[2] = 0";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceX, w = 0, MipMapLODBias[2] = 0";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceY, w = 0, MipMapLODBias[2] = 0";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; w = 0, MipMapLODBias[2] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceX, w = 0, MipMapLODBias[2] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceY, w = 0, MipMapLODBias[2] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; w = 0, MipMapLODBias[2] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceX, w = 0, MipMapLODBias[2] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceY, w = 0, MipMapLODBias[2] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; w = 0, MipMapLODBias[2] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceX, w = 0, MipMapLODBias[2] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceY, w = 0, MipMapLODBias[2] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; w = 0, MipMapLODBias[2] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceX, w = 0, MipMapLODBias[2] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceY, w = 0, MipMapLODBias[2] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; w = 0, MipMapLODBias[2] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceX, w = 0, MipMapLODBias[2] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceY, w = 0, MipMapLODBias[2] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; w = 0, MipMapLODBias[2] = 6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceX, w = 0, MipMapLODBias[2] = 6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol2; sliceY, w = 0, MipMapLODBias[2] = 6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s2
			
			texldb r1, t0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; w = 0, MipMapLODBias[2] = 0";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r0, t0
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; sliceX, w = 0, MipMapLODBias[2] = 0";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r0, t0
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; sliceY, w = 0, MipMapLODBias[2] = 0";
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r0, t0
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol2; w = 0, MipMapLODBias[2] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r1, t0
			texldb r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol2; sliceX, w = 0, MipMapLODBias[2] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r1, t0
			texldb r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol2; sliceY, w = 0, MipMapLODBias[2] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r1, t0
			texldb r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol2; w = 0, MipMapLODBias[2] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r2, t0
			texldb r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol2; sliceX, w = 0, MipMapLODBias[2] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r2, t0
			texldb r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol2; sliceY, w = 0, MipMapLODBias[2] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r2, t0
			texldb r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol2; w = 0, MipMapLODBias[2] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r3, t0
			texldb r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol2; sliceX, w = 0, MipMapLODBias[2] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r3, t0
			texldb r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol2; sliceY, w = 0, MipMapLODBias[2] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r3, t0
			texldb r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol2; w = 0, MipMapLODBias[2] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r4, t0
			texldb r1, r4, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol2; sliceX, w = 0, MipMapLODBias[2] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r4, t0
			texldb r1, r4, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol2; sliceY, w = 0, MipMapLODBias[2] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r4, t0
			texldb r1, r4, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol2; w = 0, MipMapLODBias[2] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r5, t0
			texldb r1, r5, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol2; sliceX, w = 0, MipMapLODBias[2] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r5, t0
			texldb r1, r5, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol2; sliceY, w = 0, MipMapLODBias[2] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r5, t0
			texldb r1, r5, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol2; w = 0, MipMapLODBias[2] = 6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r6, t0
			texldb r1, r6, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol2; sliceX, w = 0, MipMapLODBias[2] = 6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r6, t0
			texldb r1, r6, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol2; sliceY, w = 0, MipMapLODBias[2] = 6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s2
			
			mov r6, t0
			texldb r1, r6, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; w = 3, MipMapLODBias[2] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r0, t2
			mov	r0.w,	c0.w
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; sliceX, w = 3, MipMapLODBias[2] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r0, t2
			mov	r0.w,	c0.w
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol2; sliceY, w = 3, MipMapLODBias[2] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r0, t2
			mov	r0.w,	c0.w
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol2; w = 3, MipMapLODBias[2] = -1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r1, t2
			mov	r1.w,	c0.w
			texldb r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol2; sliceX, w = 3, MipMapLODBias[2] = -1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r1, t2
			mov	r1.w,	c0.w
			texldb r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol2; sliceY, w = 3, MipMapLODBias[2] = -1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r1, t2
			mov	r1.w,	c0.w
			texldb r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol2; w = 3, MipMapLODBias[2] = -2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r2, t2
			mov	r2.w,	c0.w
			texldb r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol2; sliceX, w = 3, MipMapLODBias[2] = -2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r2, t2
			mov	r2.w,	c0.w
			texldb r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol2; sliceY, w = 3, MipMapLODBias[2] = -2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r2, t2
			mov	r2.w,	c0.w
			texldb r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol2; w = 3, MipMapLODBias[2] = -3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r3, t2
			mov	r3.w,	c0.w
			texldb r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol2; sliceX, w = 3, MipMapLODBias[2] = -3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r3, t2
			mov	r3.w,	c0.w
			texldb r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol2; sliceY, w = 3, MipMapLODBias[2] = -3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r3, t2
			mov	r3.w,	c0.w
			texldb r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol2; w = 3, MipMapLODBias[2] = -4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r4, t2
			mov	r4.w,	c0.w
			texldb r1, r4, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol2; sliceX, w = 3, MipMapLODBias[2] = -4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r4, t2
			mov	r4.w,	c0.w
			texldb r1, r4, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol2; sliceY, w = 3, MipMapLODBias[2] = -4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r4, t2
			mov	r4.w,	c0.w
			texldb r1, r4, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol2; w = 3, MipMapLODBias[2] = -5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r5, t2
			mov	r5.w,	c0.w
			texldb r1, r5, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol2; sliceX, w = 3, MipMapLODBias[2] = -5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r5, t2
			mov	r5.w,	c0.w
			texldb r1, r5, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol2; sliceY, w = 3, MipMapLODBias[2] = -5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r5, t2
			mov	r5.w,	c0.w
			texldb r1, r5, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol2; w = 3, MipMapLODBias[2] = -6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r6, t2
			mov	r6.w,	c0.w
			texldb r1, r6, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol2; sliceX, w = 3, MipMapLODBias[2] = -6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r6, t2
			mov	r6.w,	c0.w
			texldb r1, r6, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol2; sliceY, w = 3, MipMapLODBias[2] = -6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s2
			
			mov r6, t2
			mov	r6.w,	c0.w
			texldb r1, r6, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t1, TexVol10";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			texldb r1, t1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t1, TexVol10, SliceX";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			texldb r1, t1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t1, TexVol10, SliceY";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			texldb r1, t1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			mov r1, t1
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; SliceX";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			mov r1, t1
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; SliceY";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			mov r1, t1
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t1, TexVol10, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			texldb r1.xyzw, t1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t1, TexVol10; SliceX, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			texldb r1.xyzw, t1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t1, TexVol10; SliceY, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			texldb r1.xyzw, t1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			mov r1, t1
			texldb r1.xyzw, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; SliceX, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			mov r1, t1
			texldb r1.xyzw, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; SliceY, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			mov r1, t1
			texldb r1.xyzw, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : t1, TexVol10";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			texldb_pp r1, t1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : t1, TexVol10, SliceX";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			texldb_pp r1, t1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : t1, TexVol10, SliceY";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			texldb_pp r1, t1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : r1, TexVol10";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			mov r1, t1
			texldb_pp r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : r1, TexVol10; SliceX";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			mov r1, t1
			texldb_pp r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : r1, TexVol10; SliceY";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s10
			
			mov r1, t1
			texldb_pp r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol10; w = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol10; sliceX, w = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol10; sliceY, w = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; w = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 1.0
            dcl_volume s10
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; sliceX, w = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 1.0
            dcl_volume s10
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; sliceY, w = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 1.0
            dcl_volume s10
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol10; w = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 2.0
            dcl_volume s10
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol10; sliceX, w = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 2.0
            dcl_volume s10
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol10; sliceY, w = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 2.0
            dcl_volume s10
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol10; w = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 3.0
            dcl_volume s10
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol10; sliceX, w = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 3.0
            dcl_volume s10
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol10; sliceY, w = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 3.0
            dcl_volume s10
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol10; level = 5, w = -0";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_1>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -0.0
            dcl_volume s10
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol10; sliceX, level = 5, w = -0";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_1>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -0.0
            dcl_volume s10
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol10; sliceY, level = 5, w = -0";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_1>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -0.0
            dcl_volume s10
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; level = 5, w = -1";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_1>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -1.0
            dcl_volume s10
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; sliceX, level = 5, w = -1";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_1>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -1.0
            dcl_volume s10
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; sliceY, level = 5, w = -1";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_1>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -1.0
            dcl_volume s10
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol10; level = 5, w = -2";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_1>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -2.0
            dcl_volume s10
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol10; sliceX, level = 5, w = -2";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_1>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -2.0
            dcl_volume s10
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol10; sliceY, level = 5, w = -2";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_1>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -2.0
            dcl_volume s10
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol10; level = 5, w = -3";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_1>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -3.0
            dcl_volume s10
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol10; sliceX, level = 5, w = -3";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_1>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -3.0
            dcl_volume s10
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol10; sliceY, level = 5, w = -3";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_1>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -3.0
            dcl_volume s10
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; w = 0, MaxMipLevel[10] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceX, w = 0, MaxMipLevel[10] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceY, w = 0, MaxMipLevel[10] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; w = 0, MaxMipLevel[10] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceX, w = 0, MaxMipLevel[10] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceY, w = 0, MaxMipLevel[10] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; w = 0, MaxMipLevel[10] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceX, w = 0, MaxMipLevel[10] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceY, w = 0, MaxMipLevel[10] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; w = 0, MaxMipLevel[10] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceX, w = 0, MaxMipLevel[10] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceY, w = 0, MaxMipLevel[10] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; w = 0, MaxMipLevel[10] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceX, w = 0, MaxMipLevel[10] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceY, w = 0, MaxMipLevel[10] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; w = 0, MaxMipLevel[10] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceX, w = 0, MaxMipLevel[10] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceY, w = 0, MaxMipLevel[10] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; w = 0, MaxMipLevel[10] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 6;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceX, w = 0, MaxMipLevel[10] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 6;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceY, w = 0, MaxMipLevel[10] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 6;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol10; w = 0, MaxMipLevel[10] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r0, t0
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol10; sliceX, w = 0, MaxMipLevel[10] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r0, t0
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol10; sliceY, w = 0, MaxMipLevel[10] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r0, t0
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; w = 0, MaxMipLevel[10] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r1, t0
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; sliceX, w = 0, MaxMipLevel[10] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r1, t0
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; sliceY, w = 0, MaxMipLevel[10] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r1, t0
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol10; w = 0, MaxMipLevel[10] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r2, t0
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol10; sliceX, w = 0, MaxMipLevel[10] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r2, t0
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol10; sliceY, w = 0, MaxMipLevel[10] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r2, t0
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol10; w = 0, MaxMipLevel[10] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r3, t0
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol10; sliceX, w = 0, MaxMipLevel[10] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r3, t0
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol10; sliceY, w = 0, MaxMipLevel[10] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r3, t0
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol10; w = 0, MaxMipLevel[10] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r4, t0
			texldb r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol10; sliceX, w = 0, MaxMipLevel[10] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r4, t0
			texldb r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol10; sliceY, w = 0, MaxMipLevel[10] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r4, t0
			texldb r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol10; w = 0, MaxMipLevel[10] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r5, t0
			texldb r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol10; sliceX, w = 0, MaxMipLevel[10] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r5, t0
			texldb r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol10; sliceY, w = 0, MaxMipLevel[10] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r5, t0
			texldb r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol10; w = 0, MaxMipLevel[10] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 6;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r6, t0
			texldb r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol10; sliceX, w = 0, MaxMipLevel[10] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 6;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r6, t0
			texldb r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol10; sliceY, w = 0, MaxMipLevel[10] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 6;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r6, t0
			texldb r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; w = 0, MipMapLODBias[10] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceX, w = 0, MipMapLODBias[10] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceY, w = 0, MipMapLODBias[10] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; w = 0, MipMapLODBias[10] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceX, w = 0, MipMapLODBias[10] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceY, w = 0, MipMapLODBias[10] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; w = 0, MipMapLODBias[10] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceX, w = 0, MipMapLODBias[10] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceY, w = 0, MipMapLODBias[10] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; w = 0, MipMapLODBias[10] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceX, w = 0, MipMapLODBias[10] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceY, w = 0, MipMapLODBias[10] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; w = 0, MipMapLODBias[10] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceX, w = 0, MipMapLODBias[10] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceY, w = 0, MipMapLODBias[10] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; w = 0, MipMapLODBias[10] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceX, w = 0, MipMapLODBias[10] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceY, w = 0, MipMapLODBias[10] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; w = 0, MipMapLODBias[10] = 6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceX, w = 0, MipMapLODBias[10] = 6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol10; sliceY, w = 0, MipMapLODBias[10] = 6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			texldb r1, t0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol10; w = 0, MipMapLODBias[10] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r0, t0
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol10; sliceX, w = 0, MipMapLODBias[10] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r0, t0
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol10; sliceY, w = 0, MipMapLODBias[10] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r0, t0
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; w = 0, MipMapLODBias[10] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r1, t0
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; sliceX, w = 0, MipMapLODBias[10] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r1, t0
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; sliceY, w = 0, MipMapLODBias[10] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r1, t0
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol10; w = 0, MipMapLODBias[10] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r2, t0
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol10; sliceX, w = 0, MipMapLODBias[10] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r2, t0
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol10; sliceY, w = 0, MipMapLODBias[10] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r2, t0
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol10; w = 0, MipMapLODBias[10] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r3, t0
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol10; sliceX, w = 0, MipMapLODBias[10] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r3, t0
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol10; sliceY, w = 0, MipMapLODBias[10] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r3, t0
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol10; w = 0, MipMapLODBias[10] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r4, t0
			texldb r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol10; sliceX, w = 0, MipMapLODBias[10] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r4, t0
			texldb r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol10; sliceY, w = 0, MipMapLODBias[10] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r4, t0
			texldb r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol10; w = 0, MipMapLODBias[10] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r5, t0
			texldb r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol10; sliceX, w = 0, MipMapLODBias[10] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r5, t0
			texldb r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol10; sliceY, w = 0, MipMapLODBias[10] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r5, t0
			texldb r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol10; w = 0, MipMapLODBias[10] = 6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r6, t0
			texldb r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol10; sliceX, w = 0, MipMapLODBias[10] = 6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r6, t0
			texldb r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol10; sliceY, w = 0, MipMapLODBias[10] = 6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s10
			
			mov r6, t0
			texldb r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol10; w = 3, MipMapLODBias[10] = -0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r0, t2
			mov	r0.w,	c0.w
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol10; sliceX, w = 3, MipMapLODBias[10] = -0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r0, t2
			mov	r0.w,	c0.w
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol10; sliceY, w = 3, MipMapLODBias[10] = -0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r0, t2
			mov	r0.w,	c0.w
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; w = 3, MipMapLODBias[10] = -1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r1, t2
			mov	r1.w,	c0.w
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; sliceX, w = 3, MipMapLODBias[10] = -1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r1, t2
			mov	r1.w,	c0.w
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol10; sliceY, w = 3, MipMapLODBias[10] = -1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r1, t2
			mov	r1.w,	c0.w
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol10; w = 3, MipMapLODBias[10] = -2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r2, t2
			mov	r2.w,	c0.w
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol10; sliceX, w = 3, MipMapLODBias[10] = -2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r2, t2
			mov	r2.w,	c0.w
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol10; sliceY, w = 3, MipMapLODBias[10] = -2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r2, t2
			mov	r2.w,	c0.w
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol10; w = 3, MipMapLODBias[10] = -3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r3, t2
			mov	r3.w,	c0.w
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol10; sliceX, w = 3, MipMapLODBias[10] = -3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r3, t2
			mov	r3.w,	c0.w
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol10; sliceY, w = 3, MipMapLODBias[10] = -3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r3, t2
			mov	r3.w,	c0.w
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol10; w = 3, MipMapLODBias[10] = -4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r4, t2
			mov	r4.w,	c0.w
			texldb r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol10; sliceX, w = 3, MipMapLODBias[10] = -4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r4, t2
			mov	r4.w,	c0.w
			texldb r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol10; sliceY, w = 3, MipMapLODBias[10] = -4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r4, t2
			mov	r4.w,	c0.w
			texldb r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol10; w = 3, MipMapLODBias[10] = -5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r5, t2
			mov	r5.w,	c0.w
			texldb r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol10; sliceX, w = 3, MipMapLODBias[10] = -5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r5, t2
			mov	r5.w,	c0.w
			texldb r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol10; sliceY, w = 3, MipMapLODBias[10] = -5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r5, t2
			mov	r5.w,	c0.w
			texldb r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol10; w = 3, MipMapLODBias[10] = -6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r6, t2
			mov	r6.w,	c0.w
			texldb r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol10; sliceX, w = 3, MipMapLODBias[10] = -6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r6, t2
			mov	r6.w,	c0.w
			texldb r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol10; sliceY, w = 3, MipMapLODBias[10] = -6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s10
			
			mov r6, t2
			mov	r6.w,	c0.w
			texldb r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t2, TexVol15";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			texldb r1, t2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t2, TexVol15, SliceX";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			texldb r1, t2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t2, TexVol15, SliceY";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			texldb r1, t2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			mov r2, t2
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; SliceX";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			mov r2, t2
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; SliceY";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			mov r2, t2
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t2, TexVol15, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			texldb r1.xyzw, t2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t2, TexVol15; SliceX, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			texldb r1.xyzw, t2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t2, TexVol15; SliceY, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			texldb r1.xyzw, t2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			mov r2, t2
			texldb r1.xyzw, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; SliceX, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			mov r2, t2
			texldb r1.xyzw, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; SliceY, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			mov r2, t2
			texldb r1.xyzw, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : t2, TexVol15";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			texldb_pp r1, t2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : t2, TexVol15, SliceX";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			texldb_pp r1, t2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : t2, TexVol15, SliceY";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			texldb_pp r1, t2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : r2, TexVol15";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			mov r2, t2
			texldb_pp r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : r2, TexVol15; SliceX";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			mov r2, t2
			texldb_pp r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : r2, TexVol15; SliceY";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s15
			
			mov r2, t2
			texldb_pp r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol15; w = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol15; sliceX, w = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol15; sliceY, w = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol15; w = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 1.0
            dcl_volume s15
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol15; sliceX, w = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 1.0
            dcl_volume s15
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol15; sliceY, w = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 1.0
            dcl_volume s15
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; w = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 2.0
            dcl_volume s15
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; sliceX, w = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 2.0
            dcl_volume s15
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; sliceY, w = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 2.0
            dcl_volume s15
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol15; w = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 3.0
            dcl_volume s15
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol15; sliceX, w = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 3.0
            dcl_volume s15
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol15; sliceY, w = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 3.0
            dcl_volume s15
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol15; level = 5, w = -0";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_1>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -0.0
            dcl_volume s15
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol15; sliceX, level = 5, w = -0";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_1>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -0.0
            dcl_volume s15
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol15; sliceY, level = 5, w = -0";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_1>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -0.0
            dcl_volume s15
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol15; level = 5, w = -1";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_1>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -1.0
            dcl_volume s15
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol15; sliceX, level = 5, w = -1";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_1>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -1.0
            dcl_volume s15
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol15; sliceY, level = 5, w = -1";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_1>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -1.0
            dcl_volume s15
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; level = 5, w = -2";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_1>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -2.0
            dcl_volume s15
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; sliceX, level = 5, w = -2";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_1>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -2.0
            dcl_volume s15
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; sliceY, level = 5, w = -2";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_1>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -2.0
            dcl_volume s15
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol15; level = 5, w = -3";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_1>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -3.0
            dcl_volume s15
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol15; sliceX, level = 5, w = -3";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_1>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -3.0
            dcl_volume s15
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol15; sliceY, level = 5, w = -3";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_1>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, -3.0
            dcl_volume s15
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; w = 0, MaxMipLevel[15] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceX, w = 0, MaxMipLevel[15] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceY, w = 0, MaxMipLevel[15] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; w = 0, MaxMipLevel[15] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceX, w = 0, MaxMipLevel[15] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceY, w = 0, MaxMipLevel[15] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; w = 0, MaxMipLevel[15] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceX, w = 0, MaxMipLevel[15] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceY, w = 0, MaxMipLevel[15] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; w = 0, MaxMipLevel[15] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceX, w = 0, MaxMipLevel[15] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceY, w = 0, MaxMipLevel[15] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; w = 0, MaxMipLevel[15] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceX, w = 0, MaxMipLevel[15] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceY, w = 0, MaxMipLevel[15] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; w = 0, MaxMipLevel[15] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceX, w = 0, MaxMipLevel[15] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceY, w = 0, MaxMipLevel[15] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; w = 0, MaxMipLevel[15] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 6;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceX, w = 0, MaxMipLevel[15] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 6;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceY, w = 0, MaxMipLevel[15] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 6;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol15; w = 0, MaxMipLevel[15] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r0, t0
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol15; sliceX, w = 0, MaxMipLevel[15] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r0, t0
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol15; sliceY, w = 0, MaxMipLevel[15] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r0, t0
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol15; w = 0, MaxMipLevel[15] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r1, t0
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol15; sliceX, w = 0, MaxMipLevel[15] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r1, t0
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol15; sliceY, w = 0, MaxMipLevel[15] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r1, t0
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; w = 0, MaxMipLevel[15] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r2, t0
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; sliceX, w = 0, MaxMipLevel[15] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r2, t0
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; sliceY, w = 0, MaxMipLevel[15] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r2, t0
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol15; w = 0, MaxMipLevel[15] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r3, t0
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol15; sliceX, w = 0, MaxMipLevel[15] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r3, t0
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol15; sliceY, w = 0, MaxMipLevel[15] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r3, t0
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol15; w = 0, MaxMipLevel[15] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r4, t0
			texldb r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol15; sliceX, w = 0, MaxMipLevel[15] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r4, t0
			texldb r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol15; sliceY, w = 0, MaxMipLevel[15] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r4, t0
			texldb r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol15; w = 0, MaxMipLevel[15] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r5, t0
			texldb r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol15; sliceX, w = 0, MaxMipLevel[15] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r5, t0
			texldb r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol15; sliceY, w = 0, MaxMipLevel[15] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r5, t0
			texldb r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol15; w = 0, MaxMipLevel[15] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 6;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r6, t0
			texldb r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol15; sliceX, w = 0, MaxMipLevel[15] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 6;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r6, t0
			texldb r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol15; sliceY, w = 0, MaxMipLevel[15] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 6;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r6, t0
			texldb r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; w = 0, MipMapLODBias[15] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceX, w = 0, MipMapLODBias[15] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceY, w = 0, MipMapLODBias[15] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; w = 0, MipMapLODBias[15] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceX, w = 0, MipMapLODBias[15] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceY, w = 0, MipMapLODBias[15] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; w = 0, MipMapLODBias[15] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceX, w = 0, MipMapLODBias[15] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceY, w = 0, MipMapLODBias[15] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; w = 0, MipMapLODBias[15] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceX, w = 0, MipMapLODBias[15] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceY, w = 0, MipMapLODBias[15] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; w = 0, MipMapLODBias[15] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceX, w = 0, MipMapLODBias[15] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceY, w = 0, MipMapLODBias[15] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; w = 0, MipMapLODBias[15] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceX, w = 0, MipMapLODBias[15] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceY, w = 0, MipMapLODBias[15] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; w = 0, MipMapLODBias[15] = 6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceX, w = 0, MipMapLODBias[15] = 6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexVol15; sliceY, w = 0, MipMapLODBias[15] = 6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			texldb r1, t0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol15; w = 0, MipMapLODBias[15] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r0, t0
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol15; sliceX, w = 0, MipMapLODBias[15] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r0, t0
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol15; sliceY, w = 0, MipMapLODBias[15] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r0, t0
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol15; w = 0, MipMapLODBias[15] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r1, t0
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol15; sliceX, w = 0, MipMapLODBias[15] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r1, t0
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol15; sliceY, w = 0, MipMapLODBias[15] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r1, t0
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; w = 0, MipMapLODBias[15] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r2, t0
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; sliceX, w = 0, MipMapLODBias[15] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r2, t0
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; sliceY, w = 0, MipMapLODBias[15] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r2, t0
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol15; w = 0, MipMapLODBias[15] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r3, t0
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol15; sliceX, w = 0, MipMapLODBias[15] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r3, t0
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol15; sliceY, w = 0, MipMapLODBias[15] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r3, t0
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol15; w = 0, MipMapLODBias[15] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r4, t0
			texldb r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol15; sliceX, w = 0, MipMapLODBias[15] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r4, t0
			texldb r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol15; sliceY, w = 0, MipMapLODBias[15] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r4, t0
			texldb r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol15; w = 0, MipMapLODBias[15] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r5, t0
			texldb r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol15; sliceX, w = 0, MipMapLODBias[15] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r5, t0
			texldb r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol15; sliceY, w = 0, MipMapLODBias[15] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r5, t0
			texldb r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol15; w = 0, MipMapLODBias[15] = 6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r6, t0
			texldb r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol15; sliceX, w = 0, MipMapLODBias[15] = 6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r6, t0
			texldb r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol15; sliceY, w = 0, MipMapLODBias[15] = 6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_volume s15
			
			mov r6, t0
			texldb r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol15; w = 3, MipMapLODBias[15] = -0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r0, t2
			mov	r0.w,	c0.w
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol15; sliceX, w = 3, MipMapLODBias[15] = -0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r0, t2
			mov	r0.w,	c0.w
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexVol15; sliceY, w = 3, MipMapLODBias[15] = -0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r0, t2
			mov	r0.w,	c0.w
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol15; w = 3, MipMapLODBias[15] = -1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r1, t2
			mov	r1.w,	c0.w
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol15; sliceX, w = 3, MipMapLODBias[15] = -1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r1, t2
			mov	r1.w,	c0.w
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexVol15; sliceY, w = 3, MipMapLODBias[15] = -1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r1, t2
			mov	r1.w,	c0.w
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; w = 3, MipMapLODBias[15] = -2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r2, t2
			mov	r2.w,	c0.w
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; sliceX, w = 3, MipMapLODBias[15] = -2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r2, t2
			mov	r2.w,	c0.w
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexVol15; sliceY, w = 3, MipMapLODBias[15] = -2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r2, t2
			mov	r2.w,	c0.w
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol15; w = 3, MipMapLODBias[15] = -3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r3, t2
			mov	r3.w,	c0.w
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol15; sliceX, w = 3, MipMapLODBias[15] = -3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r3, t2
			mov	r3.w,	c0.w
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexVol15; sliceY, w = 3, MipMapLODBias[15] = -3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r3, t2
			mov	r3.w,	c0.w
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol15; w = 3, MipMapLODBias[15] = -4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r4, t2
			mov	r4.w,	c0.w
			texldb r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol15; sliceX, w = 3, MipMapLODBias[15] = -4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r4, t2
			mov	r4.w,	c0.w
			texldb r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexVol15; sliceY, w = 3, MipMapLODBias[15] = -4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r4, t2
			mov	r4.w,	c0.w
			texldb r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol15; w = 3, MipMapLODBias[15] = -5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r5, t2
			mov	r5.w,	c0.w
			texldb r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol15; sliceX, w = 3, MipMapLODBias[15] = -5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r5, t2
			mov	r5.w,	c0.w
			texldb r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexVol15; sliceY, w = 3, MipMapLODBias[15] = -5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r5, t2
			mov	r5.w,	c0.w
			texldb r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol15; w = 3, MipMapLODBias[15] = -6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r6, t2
			mov	r6.w,	c0.w
			texldb r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol15; sliceX, w = 3, MipMapLODBias[15] = -6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r6, t2
			mov	r6.w,	c0.w
			texldb r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexVol15; sliceY, w = 3, MipMapLODBias[15] = -6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_volume s15
			
			mov r6, t2
			mov	r6.w,	c0.w
			texldb r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
