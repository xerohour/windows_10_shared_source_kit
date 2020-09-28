#include "include.fx"

Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 0";
	String	Shape = "VolTLZ0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 0";
	String	Shape = "VolTLX0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 0";
	String	Shape = "VolTLY0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 1";
	String	Shape = "VolTLZ1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 1";
	String	Shape = "VolTLX1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 1";
	String	Shape = "VolTLY1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 2";
	String	Shape = "VolTLZ2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 2";
	String	Shape = "VolTLX2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 2";
	String	Shape = "VolTLY2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 3";
	String	Shape = "VolTLZ3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 3";
	String	Shape = "VolTLX3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 3";
	String	Shape = "VolTLY3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 4";
	String	Shape = "VolTLZ4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 4";
	String	Shape = "VolTLX4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 4";
	String	Shape = "VolTLY4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 5";
	String	Shape = "VolTLZ5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 5";
	String	Shape = "VolTLX5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 5";
	String	Shape = "VolTLY5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 0, writemask=xyzw";
	String	Shape = "VolTLZ0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 0, writemask=xyzw";
	String	Shape = "VolTLX0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 0, writemask=xyzw";
	String	Shape = "VolTLY0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 1, writemask=xyzw";
	String	Shape = "VolTLZ1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 1, writemask=xyzw";
	String	Shape = "VolTLX1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 1, writemask=xyzw";
	String	Shape = "VolTLY1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 2, writemask=xyzw";
	String	Shape = "VolTLZ2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 2, writemask=xyzw";
	String	Shape = "VolTLX2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 2, writemask=xyzw";
	String	Shape = "VolTLY2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 3, writemask=xyzw";
	String	Shape = "VolTLZ3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 3, writemask=xyzw";
	String	Shape = "VolTLX3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 3, writemask=xyzw";
	String	Shape = "VolTLY3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 4, writemask=xyzw";
	String	Shape = "VolTLZ4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 4, writemask=xyzw";
	String	Shape = "VolTLX4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 4, writemask=xyzw";
	String	Shape = "VolTLY4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 5, writemask=xyzw";
	String	Shape = "VolTLZ5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 5, writemask=xyzw";
	String	Shape = "VolTLX5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 5, writemask=xyzw";
	String	Shape = "VolTLY5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0.xyzw, t0, s4
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, MaxMipLevel[4] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, MaxMipLevel[4] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, MaxMipLevel[4] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, MaxMipLevel[4] = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, MaxMipLevel[4] = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, MaxMipLevel[4] = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, MaxMipLevel[4] = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, MaxMipLevel[4] = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, MaxMipLevel[4] = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, MaxMipLevel[4] = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, MaxMipLevel[4] = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, MaxMipLevel[4] = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, MaxMipLevel[4] = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, MaxMipLevel[4] = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, MaxMipLevel[4] = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, MaxMipLevel[4] = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, MaxMipLevel[4] = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, MaxMipLevel[4] = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, MipMapLODBias[4] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, MipMapLODBias[4] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, MipMapLODBias[4] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, MipMapLODBias[4] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, MipMapLODBias[4] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, MipMapLODBias[4] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, MipMapLODBias[4] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, MipMapLODBias[4] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, MipMapLODBias[4] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, MipMapLODBias[4] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, MipMapLODBias[4] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, MipMapLODBias[4] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, MipMapLODBias[4] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, MipMapLODBias[4] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, MipMapLODBias[4] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, MipMapLODBias[4] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, MipMapLODBias[4] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, MipMapLODBias[4] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 5, MipMapLODBias[4] = -0";
	String	Shape = "VolTLZ5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 5, MipMapLODBias[4] = -0";
	String	Shape = "VolTLX5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 5, MipMapLODBias[4] = -0";
	String	Shape = "VolTLY5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 5, MipMapLODBias[4] = -1";
	String	Shape = "VolTLZ5";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 5, MipMapLODBias[4] = -1";
	String	Shape = "VolTLX5";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 5, MipMapLODBias[4] = -1";
	String	Shape = "VolTLY5";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 5, MipMapLODBias[4] = -2";
	String	Shape = "VolTLZ5";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 5, MipMapLODBias[4] = -2";
	String	Shape = "VolTLX5";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 5, MipMapLODBias[4] = -2";
	String	Shape = "VolTLY5";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 5, MipMapLODBias[4] = -3";
	String	Shape = "VolTLZ5";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 5, MipMapLODBias[4] = -3";
	String	Shape = "VolTLX5";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 5, MipMapLODBias[4] = -3";
	String	Shape = "VolTLY5";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 5, MipMapLODBias[4] = -4";
	String	Shape = "VolTLZ5";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 5, MipMapLODBias[4] = -4";
	String	Shape = "VolTLX5";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 5, MipMapLODBias[4] = -4";
	String	Shape = "VolTLY5";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, level = 5, MipMapLODBias[4] = -5";
	String	Shape = "VolTLZ5";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceX, level = 5, MipMapLODBias[4] = -5";
	String	Shape = "VolTLX5";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol4, sliceY, level = 5, MipMapLODBias[4] = -5";
	String	Shape = "VolTLY5";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s4
			
			texld r0, t0, s4
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 4, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 4, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 4, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 5, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 5, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 5, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0.xyzw, t0, s8
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, MaxMipLevel[8] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, MaxMipLevel[8] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, MaxMipLevel[8] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, MaxMipLevel[8] = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, MaxMipLevel[8] = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, MaxMipLevel[8] = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, MaxMipLevel[8] = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, MaxMipLevel[8] = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, MaxMipLevel[8] = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, MaxMipLevel[8] = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, MaxMipLevel[8] = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, MaxMipLevel[8] = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, MaxMipLevel[8] = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, MaxMipLevel[8] = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, MaxMipLevel[8] = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, MaxMipLevel[8] = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, MaxMipLevel[8] = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, MaxMipLevel[8] = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, MipMapLODBias[8] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, MipMapLODBias[8] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, MipMapLODBias[8] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, MipMapLODBias[8] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, MipMapLODBias[8] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, MipMapLODBias[8] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, MipMapLODBias[8] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, MipMapLODBias[8] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, MipMapLODBias[8] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, MipMapLODBias[8] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, MipMapLODBias[8] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, MipMapLODBias[8] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, MipMapLODBias[8] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, MipMapLODBias[8] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, MipMapLODBias[8] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, MipMapLODBias[8] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, MipMapLODBias[8] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, MipMapLODBias[8] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 5, MipMapLODBias[8] = -0";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 5, MipMapLODBias[8] = -0";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 5, MipMapLODBias[8] = -0";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 5, MipMapLODBias[8] = -1";
	String	Shape = "VolTLZ5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 5, MipMapLODBias[8] = -1";
	String	Shape = "VolTLX5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 5, MipMapLODBias[8] = -1";
	String	Shape = "VolTLY5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 5, MipMapLODBias[8] = -2";
	String	Shape = "VolTLZ5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 5, MipMapLODBias[8] = -2";
	String	Shape = "VolTLX5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 5, MipMapLODBias[8] = -2";
	String	Shape = "VolTLY5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 5, MipMapLODBias[8] = -3";
	String	Shape = "VolTLZ5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 5, MipMapLODBias[8] = -3";
	String	Shape = "VolTLX5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 5, MipMapLODBias[8] = -3";
	String	Shape = "VolTLY5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 5, MipMapLODBias[8] = -4";
	String	Shape = "VolTLZ5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 5, MipMapLODBias[8] = -4";
	String	Shape = "VolTLX5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 5, MipMapLODBias[8] = -4";
	String	Shape = "VolTLY5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, level = 5, MipMapLODBias[8] = -5";
	String	Shape = "VolTLZ5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceX, level = 5, MipMapLODBias[8] = -5";
	String	Shape = "VolTLX5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol8, sliceY, level = 5, MipMapLODBias[8] = -5";
	String	Shape = "VolTLY5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s8
			
			texld r0, t0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 4, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 4, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 4, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 5, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 5, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 5, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0.xyzw, t0, s13
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, MaxMipLevel[13] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, MaxMipLevel[13] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, MaxMipLevel[13] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, MaxMipLevel[13] = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, MaxMipLevel[13] = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, MaxMipLevel[13] = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, MaxMipLevel[13] = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, MaxMipLevel[13] = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, MaxMipLevel[13] = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, MaxMipLevel[13] = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, MaxMipLevel[13] = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, MaxMipLevel[13] = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, MaxMipLevel[13] = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, MaxMipLevel[13] = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, MaxMipLevel[13] = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, MaxMipLevel[13] = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, MaxMipLevel[13] = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, MaxMipLevel[13] = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, MipMapLODBias[13] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, MipMapLODBias[13] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, MipMapLODBias[13] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, MipMapLODBias[13] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, MipMapLODBias[13] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, MipMapLODBias[13] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, MipMapLODBias[13] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, MipMapLODBias[13] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, MipMapLODBias[13] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, MipMapLODBias[13] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, MipMapLODBias[13] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, MipMapLODBias[13] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, MipMapLODBias[13] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, MipMapLODBias[13] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, MipMapLODBias[13] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, MipMapLODBias[13] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, MipMapLODBias[13] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, MipMapLODBias[13] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 5, MipMapLODBias[13] = -0";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 5, MipMapLODBias[13] = -0";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 5, MipMapLODBias[13] = -0";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 5, MipMapLODBias[13] = -1";
	String	Shape = "VolTLZ5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 5, MipMapLODBias[13] = -1";
	String	Shape = "VolTLX5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 5, MipMapLODBias[13] = -1";
	String	Shape = "VolTLY5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 5, MipMapLODBias[13] = -2";
	String	Shape = "VolTLZ5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 5, MipMapLODBias[13] = -2";
	String	Shape = "VolTLX5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 5, MipMapLODBias[13] = -2";
	String	Shape = "VolTLY5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 5, MipMapLODBias[13] = -3";
	String	Shape = "VolTLZ5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 5, MipMapLODBias[13] = -3";
	String	Shape = "VolTLX5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 5, MipMapLODBias[13] = -3";
	String	Shape = "VolTLY5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 5, MipMapLODBias[13] = -4";
	String	Shape = "VolTLZ5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 5, MipMapLODBias[13] = -4";
	String	Shape = "VolTLX5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 5, MipMapLODBias[13] = -4";
	String	Shape = "VolTLY5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, level = 5, MipMapLODBias[13] = -5";
	String	Shape = "VolTLZ5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceX, level = 5, MipMapLODBias[13] = -5";
	String	Shape = "VolTLX5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexVol13, sliceY, level = 5, MipMapLODBias[13] = -5";
	String	Shape = "VolTLY5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_volume s13
			
			texld r0, t0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}