#include "include.fx"

Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0(.xyw), Tex0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyw
            dcl_2d s0
			
			texldb r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r0, v0
			texldb r0, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c0, Tex0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.0f, 0.0f, 0.0f, 0.f
            dcl_2d s0
			
			texldb r0, c0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0; writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r1.xyzw, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex0; writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r0, v0
			texldb r0.xyzw, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c0, Tex0; writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.98f, 0.98f, 0.98f, 0.f
            dcl_2d s0
			
			texldb r0.xyzw, c0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : v0, Tex0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyw
            dcl_2d s0
			
			texldb_pp r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : r0, Tex0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r0, v0
			texldb_pp r0, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : c0, Tex0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.48f, 0.48f, 0.48f, 0.f
            dcl_2d s0
			
			texldb_pp r0, c0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex0; w = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex0; w = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r1, v0.xy
			mov r1.w, c0.x
			texldb r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex0; w = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_2d s0
			
			mov r2, v0.xy
			mov r2.w, c0.x
			texldb r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, Tex0; w = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_2d s0
			
			mov r3, v0.xy
			mov r3.w, c0.x
			texldb r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, Tex0; w = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_2d s0
			
			mov r4, v0.xy
			mov r4.w, c0.x
			texldb r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, Tex0; w = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s0
			
			mov r5, v0.xy
			mov r5.w, c0.x
			texldb r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, Tex0; w = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_2d s0
			
			mov r6, v0.xy
			mov r6.w, c0.x
			texldb r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex0; level = 5, w = -0";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex0; level = 5, w = -1";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_2d s0
			
			mov r1, v0.xy
			mov r1.w, c0.x
			texldb r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex0; level = 5, w = -2";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_2d s0
			
			mov r2, v0.xy
			mov r2.w, c0.x
			texldb r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, Tex0; level = 5, w = -3";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_2d s0
			
			mov r3, v0.xy
			mov r3.w, c0.x
			texldb r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, Tex0; level = 5, w = -4";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -4.0, -4.0, -4.0, -4.0
            dcl_2d s0
			
			mov r4, v0.xy
			mov r4.w, c0.x
			texldb r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, Tex0; level = 5, w = -5";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -5.0, -5.0, -5.0, -5.0
            dcl_2d s0
			
			mov r5, v0.xy
			mov r5.w, c0.x
			texldb r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, Tex0; level = 5, w = -6";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -6.0, -6.0, -6.0, -6.0
            dcl_2d s0
			
			mov r6, v0.xy
			mov r6.w, c0.x
			texldb r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0; w = 0, MaxMipLevel[0] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0; w = 0, MaxMipLevel[0] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 1;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0; w = 0, MaxMipLevel[0] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 2;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0; w = 0, MaxMipLevel[0] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 3;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0; w = 0, MaxMipLevel[0] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 4;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0; w = 0, MaxMipLevel[0] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 5;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0; w = 0, MaxMipLevel[0] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 6;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex0; w = 0, MaxMipLevel[0] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r0, v0
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex0; w = 0, MaxMipLevel[0] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 1;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r1, v0
			texldb r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex0; w = 0, MaxMipLevel[0] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 2;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r2, v0
			texldb r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, Tex0; w = 0, MaxMipLevel[0] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 3;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r3, v0
			texldb r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, Tex0; w = 0, MaxMipLevel[0] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 4;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r4, v0
			texldb r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, Tex0; w = 0, MaxMipLevel[0] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 5;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r5, v0
			texldb r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, Tex0; w = 0, MaxMipLevel[0] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 6;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r6, v0
			texldb r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0; w = 0, MipMapLODBias[0] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0; w = 0, MipMapLODBias[0] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0; w = 0, MipMapLODBias[0] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0; w = 0, MipMapLODBias[0] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0; w = 0, MipMapLODBias[0] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0; w = 0, MipMapLODBias[0] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex0; w = 0, MipMapLODBias[0] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texldb r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex0; w = 0, MipMapLODBias[0] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r0, v0
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex0; w = 0, MipMapLODBias[0] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r1, v0
			texldb r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex0; w = 0, MipMapLODBias[0] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r2, v0
			texldb r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, Tex0; w = 0, MipMapLODBias[0] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r3, v0
			texldb r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, Tex0; w = 0, MipMapLODBias[0] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r4, v0
			texldb r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, Tex0; w = 0, MipMapLODBias[0] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r5, v0
			texldb r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, Tex0; w = 0, MipMapLODBias[0] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r6, v0
			texldb r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex0; w = 5, MipMapLODBias[0] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s0
			
			texldb r1, v2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex0; w = 5, MipMapLODBias[0] = -1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s0
			
			texldb r1, v2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex0; w = 5, MipMapLODBias[0] = -2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s0
			
			texldb r1, v2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex0; w = 5, MipMapLODBias[0] = -3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s0
			
			texldb r1, v2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex0; w = 5, MipMapLODBias[0] = -4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s0
			
			texldb r1, v2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex0; w = 5, MipMapLODBias[0] = -5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s0
			
			texldb r1, v2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex0; w = 5, MipMapLODBias[0] = -6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s0
			
			texldb r1, v2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex0; w = 5, MipMapLODBias[0] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s0
			
			mov r0, v2
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex0; w = 5, MipMapLODBias[0] = -1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s0
			
			mov r1, v2
			texldb r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex0; w = 5, MipMapLODBias[0] = -2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s0
			
			mov r2, v2
			texldb r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r3, Tex0; w = 5, MipMapLODBias[0] = -3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s0
			
			mov r3, v2
			texldb r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r4, Tex0; w = 5, MipMapLODBias[0] = -4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s0
			
			mov r4, v2
			texldb r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r5, Tex0; w = 5, MipMapLODBias[0] = -5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s0
			
			mov r5, v2
			texldb r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r6, Tex0; w = 5, MipMapLODBias[0] = -6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s0
			
			mov r6, v2
			texldb r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : v1, Tex9";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s9
			
			texldb r0, v1, s9
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v1(.xyw), Tex9";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1.xyw
            dcl_2d s9
			
			texldb r0, v1, s9
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex9";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s9
			
			mov r1, v1
			texldb r0, r1, s9
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c1, Tex9";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.0f, 0.0f, 0.0f, 1.f
            dcl_2d s9
			
			texldb r0, c1, s9
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v1, Tex9; writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s9
			
			texldb r1.xyzw, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex9; writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s9
			
			mov r1, v1
			texldb r0.xyzw, r1, s9
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c1, Tex9; writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.98f, 0.98f, 0.98f, 1.f
            dcl_2d s9
			
			texldb r0.xyzw, c1, s9
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : v1, Tex9";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1.xyw
            dcl_2d s9
			
			texldb_pp r0, v1, s9
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : r1, Tex9";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s9
			
			mov r1, v1
			texldb_pp r0, r1, s9
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : c1, Tex9";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.48f, 0.48f, 0.48f, 1.f
            dcl_2d s9
			
			texldb_pp r0, c1, s9
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex9; w = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex9; w = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r1, v0.xy
			mov r1.w, c0.x
			texldb r1, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex9; w = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_2d s9
			
			mov r2, v0.xy
			mov r2.w, c0.x
			texldb r1, r2, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, Tex9; w = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_2d s9
			
			mov r3, v0.xy
			mov r3.w, c0.x
			texldb r1, r3, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, Tex9; w = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_2d s9
			
			mov r4, v0.xy
			mov r4.w, c0.x
			texldb r1, r4, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, Tex9; w = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s9
			
			mov r5, v0.xy
			mov r5.w, c0.x
			texldb r1, r5, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, Tex9; w = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_2d s9
			
			mov r6, v0.xy
			mov r6.w, c0.x
			texldb r1, r6, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex9; level = 5, w = -0";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_2>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex9; level = 5, w = -1";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_2>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_2d s9
			
			mov r1, v0.xy
			mov r1.w, c0.x
			texldb r1, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex9; level = 5, w = -2";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_2>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_2d s9
			
			mov r2, v0.xy
			mov r2.w, c0.x
			texldb r1, r2, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, Tex9; level = 5, w = -3";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_2>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_2d s9
			
			mov r3, v0.xy
			mov r3.w, c0.x
			texldb r1, r3, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, Tex9; level = 5, w = -4";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_2>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -4.0, -4.0, -4.0, -4.0
            dcl_2d s9
			
			mov r4, v0.xy
			mov r4.w, c0.x
			texldb r1, r4, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, Tex9; level = 5, w = -5";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_2>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -5.0, -5.0, -5.0, -5.0
            dcl_2d s9
			
			mov r5, v0.xy
			mov r5.w, c0.x
			texldb r1, r5, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, Tex9; level = 5, w = -6";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_2>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -6.0, -6.0, -6.0, -6.0
            dcl_2d s9
			
			mov r6, v0.xy
			mov r6.w, c0.x
			texldb r1, r6, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex9; w = 0, MaxMipLevel[9] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			texldb r1, v0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex9; w = 0, MaxMipLevel[9] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			texldb r1, v0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex9; w = 0, MaxMipLevel[9] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 2;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			texldb r1, v0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex9; w = 0, MaxMipLevel[9] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			texldb r1, v0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex9; w = 0, MaxMipLevel[9] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 4;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			texldb r1, v0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex9; w = 0, MaxMipLevel[9] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			texldb r1, v0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex9; w = 0, MaxMipLevel[9] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 6;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			texldb r1, v0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex9; w = 0, MaxMipLevel[9] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			mov r0, v0
			texldb r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex9; w = 0, MaxMipLevel[9] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			mov r1, v0
			texldb r1, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex9; w = 0, MaxMipLevel[9] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 2;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			mov r2, v0
			texldb r1, r2, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, Tex9; w = 0, MaxMipLevel[9] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			mov r3, v0
			texldb r1, r3, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, Tex9; w = 0, MaxMipLevel[9] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 4;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			mov r4, v0
			texldb r1, r4, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, Tex9; w = 0, MaxMipLevel[9] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			mov r5, v0
			texldb r1, r5, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, Tex9; w = 0, MaxMipLevel[9] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 6;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			mov r6, v0
			texldb r1, r6, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex9; w = 0, MipMapLODBias[9] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			texldb r1, v0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex9; w = 0, MipMapLODBias[9] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			texldb r1, v0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex9; w = 0, MipMapLODBias[9] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			texldb r1, v0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex9; w = 0, MipMapLODBias[9] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			texldb r1, v0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex9; w = 0, MipMapLODBias[9] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			texldb r1, v0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex9; w = 0, MipMapLODBias[9] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			texldb r1, v0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex9; w = 0, MipMapLODBias[9] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			texldb r1, v0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex9; w = 0, MipMapLODBias[9] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			mov r0, v0
			texldb r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex9; w = 0, MipMapLODBias[9] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			mov r1, v0
			texldb r1, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex9; w = 0, MipMapLODBias[9] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			mov r2, v0
			texldb r1, r2, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, Tex9; w = 0, MipMapLODBias[9] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			mov r3, v0
			texldb r1, r3, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, Tex9; w = 0, MipMapLODBias[9] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			mov r4, v0
			texldb r1, r4, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, Tex9; w = 0, MipMapLODBias[9] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			mov r5, v0
			texldb r1, r5, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, Tex9; w = 0, MipMapLODBias[9] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s9
			
			mov r6, v0
			texldb r1, r6, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex9; w = 5, MipMapLODBias[9] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s9
			
			texldb r1, v2, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex9; w = 5, MipMapLODBias[9] = -1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s9
			
			texldb r1, v2, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex9; w = 5, MipMapLODBias[9] = -2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s9
			
			texldb r1, v2, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex9; w = 5, MipMapLODBias[9] = -3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s9
			
			texldb r1, v2, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex9; w = 5, MipMapLODBias[9] = -4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s9
			
			texldb r1, v2, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex9; w = 5, MipMapLODBias[9] = -5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s9
			
			texldb r1, v2, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex9; w = 5, MipMapLODBias[9] = -6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s9
			
			texldb r1, v2, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex9; w = 5, MipMapLODBias[9] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s9
			
			mov r0, v2
			texldb r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex9; w = 5, MipMapLODBias[9] = -1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s9
			
			mov r1, v2
			texldb r1, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex9; w = 5, MipMapLODBias[9] = -2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s9
			
			mov r2, v2
			texldb r1, r2, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r3, Tex9; w = 5, MipMapLODBias[9] = -3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s9
			
			mov r3, v2
			texldb r1, r3, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r4, Tex9; w = 5, MipMapLODBias[9] = -4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s9
			
			mov r4, v2
			texldb r1, r4, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r5, Tex9; w = 5, MipMapLODBias[9] = -5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s9
			
			mov r5, v2
			texldb r1, r5, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r6, Tex9; w = 5, MipMapLODBias[9] = -6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s9
			
			mov r6, v2
			texldb r1, r6, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex14";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			texldb r0, v2, s14
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2(.xyw), Tex14";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2.xyw
            dcl_2d s14
			
			texldb r0, v2, s14
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex14";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			mov r2, v2
			texldb r0, r2, s14
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c2, Tex14";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.0f, 0.0f, 0.0f, 2.f
            dcl_2d s14
			
			texldb r0, c2, s14
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex14; writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			texldb r1.xyzw, v2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex14; writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			mov r2, v2
			texldb r0.xyzw, r2, s14
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c2, Tex14; writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.98f, 0.98f, 0.98f, 2.f
            dcl_2d s14
			
			texldb r0.xyzw, c2, s14
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : v2, Tex14";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2.xyw
            dcl_2d s14
			
			texldb_pp r0, v2, s14
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : r2, Tex14";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			mov r2, v2
			texldb_pp r0, r2, s14
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : c2, Tex14";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.48f, 0.48f, 0.48f, 2.f
            dcl_2d s14
			
			texldb_pp r0, c2, s14
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex14; w = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex14; w = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r1, v0.xy
			mov r1.w, c0.x
			texldb r1, r1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex14; w = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_2d s14
			
			mov r2, v0.xy
			mov r2.w, c0.x
			texldb r1, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, Tex14; w = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_2d s14
			
			mov r3, v0.xy
			mov r3.w, c0.x
			texldb r1, r3, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, Tex14; w = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_2d s14
			
			mov r4, v0.xy
			mov r4.w, c0.x
			texldb r1, r4, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, Tex14; w = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s14
			
			mov r5, v0.xy
			mov r5.w, c0.x
			texldb r1, r5, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, Tex14; w = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_2d s14
			
			mov r6, v0.xy
			mov r6.w, c0.x
			texldb r1, r6, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex14; level = 5, w = -0";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_2>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex14; level = 5, w = -1";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_2>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_2d s14
			
			mov r1, v0.xy
			mov r1.w, c0.x
			texldb r1, r1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex14; level = 5, w = -2";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_2>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_2d s14
			
			mov r2, v0.xy
			mov r2.w, c0.x
			texldb r1, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, Tex14; level = 5, w = -3";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_2>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_2d s14
			
			mov r3, v0.xy
			mov r3.w, c0.x
			texldb r1, r3, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, Tex14; level = 5, w = -4";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_2>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -4.0, -4.0, -4.0, -4.0
            dcl_2d s14
			
			mov r4, v0.xy
			mov r4.w, c0.x
			texldb r1, r4, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, Tex14; level = 5, w = -5";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_2>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -5.0, -5.0, -5.0, -5.0
            dcl_2d s14
			
			mov r5, v0.xy
			mov r5.w, c0.x
			texldb r1, r5, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, Tex14; level = 5, w = -6";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_2>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -6.0, -6.0, -6.0, -6.0
            dcl_2d s14
			
			mov r6, v0.xy
			mov r6.w, c0.x
			texldb r1, r6, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex14; w = 0, MaxMipLevel[14] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			texldb r1, v0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex14; w = 0, MaxMipLevel[14] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 1;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			texldb r1, v0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex14; w = 0, MaxMipLevel[14] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			texldb r1, v0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex14; w = 0, MaxMipLevel[14] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 3;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			texldb r1, v0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex14; w = 0, MaxMipLevel[14] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 4;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			texldb r1, v0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex14; w = 0, MaxMipLevel[14] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 5;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			texldb r1, v0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex14; w = 0, MaxMipLevel[14] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 6;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			texldb r1, v0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex14; w = 0, MaxMipLevel[14] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			mov r0, v0
			texldb r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex14; w = 0, MaxMipLevel[14] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 1;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			mov r1, v0
			texldb r1, r1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex14; w = 0, MaxMipLevel[14] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			mov r2, v0
			texldb r1, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, Tex14; w = 0, MaxMipLevel[14] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 3;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			mov r3, v0
			texldb r1, r3, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, Tex14; w = 0, MaxMipLevel[14] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 4;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			mov r4, v0
			texldb r1, r4, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, Tex14; w = 0, MaxMipLevel[14] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 5;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			mov r5, v0
			texldb r1, r5, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, Tex14; w = 0, MaxMipLevel[14] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 6;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			mov r6, v0
			texldb r1, r6, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex14; w = 0, MipMapLODBias[14] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			texldb r1, v0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex14; w = 0, MipMapLODBias[14] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			texldb r1, v0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex14; w = 0, MipMapLODBias[14] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			texldb r1, v0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex14; w = 0, MipMapLODBias[14] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			texldb r1, v0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex14; w = 0, MipMapLODBias[14] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			texldb r1, v0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex14; w = 0, MipMapLODBias[14] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			texldb r1, v0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, Tex14; w = 0, MipMapLODBias[14] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			texldb r1, v0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex14; w = 0, MipMapLODBias[14] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			mov r0, v0
			texldb r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex14; w = 0, MipMapLODBias[14] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			mov r1, v0
			texldb r1, r1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex14; w = 0, MipMapLODBias[14] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			mov r2, v0
			texldb r1, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, Tex14; w = 0, MipMapLODBias[14] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			mov r3, v0
			texldb r1, r3, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, Tex14; w = 0, MipMapLODBias[14] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			mov r4, v0
			texldb r1, r4, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, Tex14; w = 0, MipMapLODBias[14] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			mov r5, v0
			texldb r1, r5, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, Tex14; w = 0, MipMapLODBias[14] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s14
			
			mov r6, v0
			texldb r1, r6, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex14; w = 5, MipMapLODBias[14] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			texldb r1, v2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex14; w = 5, MipMapLODBias[14] = -1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			texldb r1, v2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex14; w = 5, MipMapLODBias[14] = -2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			texldb r1, v2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex14; w = 5, MipMapLODBias[14] = -3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			texldb r1, v2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex14; w = 5, MipMapLODBias[14] = -4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			texldb r1, v2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex14; w = 5, MipMapLODBias[14] = -5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			texldb r1, v2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, Tex14; w = 5, MipMapLODBias[14] = -6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			texldb r1, v2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, Tex14; w = 5, MipMapLODBias[14] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			mov r0, v2
			texldb r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r1, Tex14; w = 5, MipMapLODBias[14] = -1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			mov r1, v2
			texldb r1, r1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r2, Tex14; w = 5, MipMapLODBias[14] = -2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			mov r2, v2
			texldb r1, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r3, Tex14; w = 5, MipMapLODBias[14] = -3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			mov r3, v2
			texldb r1, r3, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r4, Tex14; w = 5, MipMapLODBias[14] = -4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			mov r4, v2
			texldb r1, r4, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r5, Tex14; w = 5, MipMapLODBias[14] = -5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			mov r5, v2
			texldb r1, r5, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texldb : r6, Tex14; w = 5, MipMapLODBias[14] = -6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s14
			
			mov r6, v2
			texldb r1, r6, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}