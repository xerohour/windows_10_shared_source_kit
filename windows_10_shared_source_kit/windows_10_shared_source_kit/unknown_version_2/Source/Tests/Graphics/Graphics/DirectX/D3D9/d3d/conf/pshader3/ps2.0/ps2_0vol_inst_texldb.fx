#include "include.fx"

Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, level = 5, w = -0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, level = 5, w = -0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, level = 5, w = -0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, level = 5, w = -1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, level = 5, w = -1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, level = 5, w = -1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, level = 5, w = -2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, level = 5, w = -2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, level = 5, w = -2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, level = 5, w = -3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, level = 5, w = -3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, level = 5, w = -3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, level = 5, w = -0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, level = 5, w = -0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, level = 5, w = -0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, level = 5, w = -1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, level = 5, w = -1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, level = 5, w = -1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, level = 5, w = -2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, level = 5, w = -2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, level = 5, w = -2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, level = 5, w = -3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, level = 5, w = -3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, level = 5, w = -3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0, MaxMipLevel[2] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0, MaxMipLevel[2] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0, MaxMipLevel[2] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0, MaxMipLevel[2] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0, MaxMipLevel[2] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0, MaxMipLevel[2] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0, MaxMipLevel[2] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0, MaxMipLevel[2] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0, MaxMipLevel[2] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0, MaxMipLevel[2] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0, MaxMipLevel[2] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0, MaxMipLevel[2] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0, MaxMipLevel[2] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0, MaxMipLevel[2] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0, MaxMipLevel[2] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0, MaxMipLevel[2] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0, MaxMipLevel[2] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0, MaxMipLevel[2] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0, MaxMipLevel[2] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0, MaxMipLevel[2] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0, MaxMipLevel[2] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0, MipMapLODBias[2] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0, MipMapLODBias[2] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0, MipMapLODBias[2] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0, MipMapLODBias[2] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0, MipMapLODBias[2] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0, MipMapLODBias[2] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0, MipMapLODBias[2] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0, MipMapLODBias[2] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0, MipMapLODBias[2] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0, MipMapLODBias[2] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0, MipMapLODBias[2] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0, MipMapLODBias[2] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0, MipMapLODBias[2] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0, MipMapLODBias[2] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0, MipMapLODBias[2] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0, MipMapLODBias[2] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0, MipMapLODBias[2] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0, MipMapLODBias[2] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 0, MipMapLODBias[2] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 0, MipMapLODBias[2] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 0, MipMapLODBias[2] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 5, MipMapLODBias[2] = -0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 5, MipMapLODBias[2] = -0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 5, MipMapLODBias[2] = -0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 5, MipMapLODBias[2] = -1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 5, MipMapLODBias[2] = -1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 5, MipMapLODBias[2] = -1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 5, MipMapLODBias[2] = -2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 5, MipMapLODBias[2] = -2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 5, MipMapLODBias[2] = -2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 5, MipMapLODBias[2] = -3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 5, MipMapLODBias[2] = -3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 5, MipMapLODBias[2] = -3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 5, MipMapLODBias[2] = -4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 5, MipMapLODBias[2] = -4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 5, MipMapLODBias[2] = -4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 5, MipMapLODBias[2] = -5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 5, MipMapLODBias[2] = -5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 5, MipMapLODBias[2] = -5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, w = 5, MipMapLODBias[2] = -6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceX, w = 5, MipMapLODBias[2] = -6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol2, sliceY, w = 5, MipMapLODBias[2] = -6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s2
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, level = 5, w = -0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, level = 5, w = -0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, level = 5, w = -0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, level = 5, w = -1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, level = 5, w = -1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, level = 5, w = -1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, level = 5, w = -2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, level = 5, w = -2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, level = 5, w = -2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, level = 5, w = -3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, level = 5, w = -3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, level = 5, w = -3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, level = 5, w = -0, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, level = 5, w = -0, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, level = 5, w = -0, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, level = 5, w = -1, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, level = 5, w = -1, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, level = 5, w = -1, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, level = 5, w = -2, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, level = 5, w = -2, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, level = 5, w = -2, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, level = 5, w = -3, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, level = 5, w = -3, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, level = 5, w = -3, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0, MaxMipLevel[10] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0, MaxMipLevel[10] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0, MaxMipLevel[10] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0, MaxMipLevel[10] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0, MaxMipLevel[10] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0, MaxMipLevel[10] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0, MaxMipLevel[10] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0, MaxMipLevel[10] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0, MaxMipLevel[10] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0, MaxMipLevel[10] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0, MaxMipLevel[10] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0, MaxMipLevel[10] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0, MaxMipLevel[10] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0, MaxMipLevel[10] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0, MaxMipLevel[10] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0, MaxMipLevel[10] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0, MaxMipLevel[10] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0, MaxMipLevel[10] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0, MaxMipLevel[10] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0, MaxMipLevel[10] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0, MaxMipLevel[10] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0, MipMapLODBias[10] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0, MipMapLODBias[10] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0, MipMapLODBias[10] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0, MipMapLODBias[10] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0, MipMapLODBias[10] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0, MipMapLODBias[10] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0, MipMapLODBias[10] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0, MipMapLODBias[10] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0, MipMapLODBias[10] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0, MipMapLODBias[10] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0, MipMapLODBias[10] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0, MipMapLODBias[10] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0, MipMapLODBias[10] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0, MipMapLODBias[10] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0, MipMapLODBias[10] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0, MipMapLODBias[10] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0, MipMapLODBias[10] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0, MipMapLODBias[10] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 0, MipMapLODBias[10] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 0, MipMapLODBias[10] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 0, MipMapLODBias[10] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 5, MipMapLODBias[10] = -0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 5, MipMapLODBias[10] = -0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 5, MipMapLODBias[10] = -0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 5, MipMapLODBias[10] = -1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 5, MipMapLODBias[10] = -1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 5, MipMapLODBias[10] = -1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 5, MipMapLODBias[10] = -2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 5, MipMapLODBias[10] = -2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 5, MipMapLODBias[10] = -2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 5, MipMapLODBias[10] = -3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 5, MipMapLODBias[10] = -3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 5, MipMapLODBias[10] = -3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 5, MipMapLODBias[10] = -4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 5, MipMapLODBias[10] = -4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 5, MipMapLODBias[10] = -4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 5, MipMapLODBias[10] = -5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 5, MipMapLODBias[10] = -5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 5, MipMapLODBias[10] = -5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, w = 5, MipMapLODBias[10] = -6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceX, w = 5, MipMapLODBias[10] = -6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol10, sliceY, w = 5, MipMapLODBias[10] = -6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s10
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, level = 5, w = -0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, level = 5, w = -0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, level = 5, w = -0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, level = 5, w = -1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, level = 5, w = -1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, level = 5, w = -1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, level = 5, w = -2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, level = 5, w = -2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, level = 5, w = -2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, level = 5, w = -3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, level = 5, w = -3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, level = 5, w = -3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, level = 5, w = -0, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, level = 5, w = -0, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, level = 5, w = -0, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, level = 5, w = -1, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, level = 5, w = -1, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, level = 5, w = -1, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, level = 5, w = -2, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, level = 5, w = -2, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, level = 5, w = -2, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, level = 5, w = -3, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, level = 5, w = -3, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, level = 5, w = -3, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0, MaxMipLevel[15] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0, MaxMipLevel[15] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0, MaxMipLevel[15] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0, MaxMipLevel[15] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0, MaxMipLevel[15] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0, MaxMipLevel[15] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0, MaxMipLevel[15] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0, MaxMipLevel[15] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0, MaxMipLevel[15] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0, MaxMipLevel[15] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0, MaxMipLevel[15] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0, MaxMipLevel[15] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0, MaxMipLevel[15] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0, MaxMipLevel[15] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0, MaxMipLevel[15] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0, MaxMipLevel[15] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0, MaxMipLevel[15] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0, MaxMipLevel[15] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0, MaxMipLevel[15] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0, MaxMipLevel[15] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0, MaxMipLevel[15] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0, MipMapLODBias[15] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0, MipMapLODBias[15] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0, MipMapLODBias[15] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0, MipMapLODBias[15] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0, MipMapLODBias[15] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0, MipMapLODBias[15] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0, MipMapLODBias[15] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0, MipMapLODBias[15] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0, MipMapLODBias[15] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0, MipMapLODBias[15] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0, MipMapLODBias[15] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0, MipMapLODBias[15] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0, MipMapLODBias[15] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0, MipMapLODBias[15] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0, MipMapLODBias[15] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0, MipMapLODBias[15] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0, MipMapLODBias[15] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0, MipMapLODBias[15] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 0, MipMapLODBias[15] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 0, MipMapLODBias[15] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 0, MipMapLODBias[15] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 5, MipMapLODBias[15] = -0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 5, MipMapLODBias[15] = -0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 5, MipMapLODBias[15] = -0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 5, MipMapLODBias[15] = -1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 5, MipMapLODBias[15] = -1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 5, MipMapLODBias[15] = -1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 5, MipMapLODBias[15] = -2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 5, MipMapLODBias[15] = -2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 5, MipMapLODBias[15] = -2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 5, MipMapLODBias[15] = -3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 5, MipMapLODBias[15] = -3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 5, MipMapLODBias[15] = -3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 5, MipMapLODBias[15] = -4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 5, MipMapLODBias[15] = -4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 5, MipMapLODBias[15] = -4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 5, MipMapLODBias[15] = -5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 5, MipMapLODBias[15] = -5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 5, MipMapLODBias[15] = -5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, w = 5, MipMapLODBias[15] = -6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceX, w = 5, MipMapLODBias[15] = -6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexVol15, sliceY, w = 5, MipMapLODBias[15] = -6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s15
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
