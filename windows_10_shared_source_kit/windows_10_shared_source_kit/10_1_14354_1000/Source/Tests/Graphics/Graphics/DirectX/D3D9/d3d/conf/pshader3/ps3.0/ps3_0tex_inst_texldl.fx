#include "include.fx"

Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0";
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
			
			texldl r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0(.xyw), Tex0";
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
			
			texldl r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, Tex0";
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
			texldl r0, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : c0, Tex0";
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
			
			texldl r0, c0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0; writemask=xyzw";
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
			
			texldl r0.xyzw, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, Tex0; writemask=xyzw";
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
			texldl r0.xyzw, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : c0, Tex0; writemask=xyzw";
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
			
			texldl r0.xyzw, c0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl_pp : v0, Tex0";
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
			
			texldl_pp r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl_pp : r0, Tex0";
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
			texldl_pp r0, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl_pp : c0, Tex0";
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
			
			texldl_pp r0, c0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, Tex0; w = 0";
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
			texldl r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, Tex0; w = 1";
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
			texldl r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, Tex0; w = 2";
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
			texldl r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, Tex0; w = 3";
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
			texldl r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, Tex0; w = 4";
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
			texldl r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, Tex0; w = 5";
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
			texldl r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, Tex0; w = 6";
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
			texldl r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0; w = 0, MaxMipLevel[0] = 0";
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
			
			texldl r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0; w = 0, MaxMipLevel[0] = 1";
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
			
			texldl r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0; w = 0, MaxMipLevel[0] = 2";
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
			
			texldl r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0; w = 0, MaxMipLevel[0] = 3";
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
			
			texldl r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0; w = 0, MaxMipLevel[0] = 4";
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
			
			texldl r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0; w = 0, MaxMipLevel[0] = 5";
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
			
			texldl r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0; w = 0, MaxMipLevel[0] = 6";
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
			
			texldl r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, Tex0; w = 0, MaxMipLevel[0] = 0";
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
			texldl r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, Tex0; w = 0, MaxMipLevel[0] = 1";
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
			texldl r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, Tex0; w = 0, MaxMipLevel[0] = 2";
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
			texldl r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, Tex0; w = 0, MaxMipLevel[0] = 3";
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
			texldl r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, Tex0; w = 0, MaxMipLevel[0] = 4";
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
			texldl r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, Tex0; w = 0, MaxMipLevel[0] = 5";
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
			texldl r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, Tex0; w = 0, MaxMipLevel[0] = 6";
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
			texldl r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0; w = 0, MipMapLODBias[0] = 0";
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
			
			texldl r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0; w = 0, MipMapLODBias[0] = 1";
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
			
			texldl r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0; w = 0, MipMapLODBias[0] = 2";
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
			
			texldl r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0; w = 0, MipMapLODBias[0] = 3";
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
			
			texldl r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0; w = 0, MipMapLODBias[0] = 4";
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
			
			texldl r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0; w = 0, MipMapLODBias[0] = 5";
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
			
			texldl r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex0; w = 0, MipMapLODBias[0] = 6";
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
			
			texldl r1, v0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, Tex0; w = 0, MipMapLODBias[0] = 0";
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
			texldl r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, Tex0; w = 0, MipMapLODBias[0] = 1";
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
			texldl r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, Tex0; w = 0, MipMapLODBias[0] = 2";
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
			texldl r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, Tex0; w = 0, MipMapLODBias[0] = 3";
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
			texldl r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, Tex0; w = 0, MipMapLODBias[0] = 4";
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
			texldl r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, Tex0; w = 0, MipMapLODBias[0] = 5";
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
			texldl r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, Tex0; w = 0, MipMapLODBias[0] = 6";
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
			texldl r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex0; w = 5, MipMapLODBias[0] = -0";
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
			
			texldl r1, v2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex0; w = 5, MipMapLODBias[0] = -1";
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
			
			texldl r1, v2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex0; w = 5, MipMapLODBias[0] = -2";
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
			
			texldl r1, v2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex0; w = 5, MipMapLODBias[0] = -3";
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
			
			texldl r1, v2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex0; w = 5, MipMapLODBias[0] = -4";
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
			
			texldl r1, v2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex0; w = 5, MipMapLODBias[0] = -5";
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
			
			texldl r1, v2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex0; w = 5, MipMapLODBias[0] = -6";
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
			
			texldl r1, v2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, Tex0; w = 5, MipMapLODBias[0] = -0";
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
			texldl r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, Tex0; w = 5, MipMapLODBias[0] = -1";
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
			texldl r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, Tex0; w = 5, MipMapLODBias[0] = -2";
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
			texldl r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, Tex0; w = 5, MipMapLODBias[0] = -3";
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
			texldl r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, Tex0; w = 5, MipMapLODBias[0] = -4";
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
			texldl r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, Tex0; w = 5, MipMapLODBias[0] = -5";
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
			texldl r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, Tex0; w = 5, MipMapLODBias[0] = -6";
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
			texldl r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v1, Tex10";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s10
			
			texldl r0, v1, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v1(.xyw), Tex10";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1.xyw
            dcl_2d s10
			
			texldl r0, v1, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, Tex10";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s10
			
			mov r1, v1
			texldl r0, r1, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : c1, Tex10";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.0f, 0.0f, 0.0f, 1.f
            dcl_2d s10
			
			texldl r0, c1, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v1, Tex10; writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s10
			
			texldl r0.xyzw, v1, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, Tex10; writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s10
			
			mov r1, v1
			texldl r0.xyzw, r1, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : c1, Tex10; writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.98f, 0.98f, 0.98f, 1.f
            dcl_2d s10
			
			texldl r0.xyzw, c1, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl_pp : v1, Tex10";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s10
			
			texldl_pp r0, v1, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl_pp : r1, Tex10";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s10
			
			mov r1, v1
			texldl_pp r0, r1, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl_pp : c1, Tex10";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.48f, 0.48f, 0.48f, 1.f
            dcl_2d s10
			
			texldl_pp r0, c1, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, Tex10; w = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s10
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, Tex10; w = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s10
			
			mov r1, v0.xy
			mov r1.w, c0.x
			texldl r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, Tex10; w = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_2d s10
			
			mov r2, v0.xy
			mov r2.w, c0.x
			texldl r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, Tex10; w = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_2d s10
			
			mov r3, v0.xy
			mov r3.w, c0.x
			texldl r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, Tex10; w = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_2d s10
			
			mov r4, v0.xy
			mov r4.w, c0.x
			texldl r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, Tex10; w = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s10
			
			mov r5, v0.xy
			mov r5.w, c0.x
			texldl r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, Tex10; w = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_2d s10
			
			mov r6, v0.xy
			mov r6.w, c0.x
			texldl r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex10; w = 0, MaxMipLevel[10] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			texldl r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex10; w = 0, MaxMipLevel[10] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 1;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			texldl r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex10; w = 0, MaxMipLevel[10] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 2;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			texldl r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex10; w = 0, MaxMipLevel[10] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 3;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			texldl r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex10; w = 0, MaxMipLevel[10] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			texldl r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex10; w = 0, MaxMipLevel[10] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 5;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			texldl r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex10; w = 0, MaxMipLevel[10] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 6;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			texldl r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, Tex10; w = 0, MaxMipLevel[10] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			mov r0, v0
			texldl r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, Tex10; w = 0, MaxMipLevel[10] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 1;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			mov r1, v0
			texldl r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, Tex10; w = 0, MaxMipLevel[10] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 2;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			mov r2, v0
			texldl r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, Tex10; w = 0, MaxMipLevel[10] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 3;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			mov r3, v0
			texldl r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, Tex10; w = 0, MaxMipLevel[10] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			mov r4, v0
			texldl r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, Tex10; w = 0, MaxMipLevel[10] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 5;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			mov r5, v0
			texldl r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, Tex10; w = 0, MaxMipLevel[10] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 6;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			mov r6, v0
			texldl r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex10; w = 0, MipMapLODBias[10] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			texldl r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex10; w = 0, MipMapLODBias[10] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			texldl r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex10; w = 0, MipMapLODBias[10] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			texldl r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex10; w = 0, MipMapLODBias[10] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			texldl r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex10; w = 0, MipMapLODBias[10] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			texldl r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex10; w = 0, MipMapLODBias[10] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			texldl r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex10; w = 0, MipMapLODBias[10] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			texldl r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, Tex10; w = 0, MipMapLODBias[10] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			mov r0, v0
			texldl r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, Tex10; w = 0, MipMapLODBias[10] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			mov r1, v0
			texldl r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, Tex10; w = 0, MipMapLODBias[10] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			mov r2, v0
			texldl r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, Tex10; w = 0, MipMapLODBias[10] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			mov r3, v0
			texldl r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, Tex10; w = 0, MipMapLODBias[10] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			mov r4, v0
			texldl r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, Tex10; w = 0, MipMapLODBias[10] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			mov r5, v0
			texldl r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, Tex10; w = 0, MipMapLODBias[10] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s10
			
			mov r6, v0
			texldl r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex10; w = 5, MipMapLODBias[10] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s10
			
			texldl r1, v2, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex10; w = 5, MipMapLODBias[10] = -1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s10
			
			texldl r1, v2, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex10; w = 5, MipMapLODBias[10] = -2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s10
			
			texldl r1, v2, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex10; w = 5, MipMapLODBias[10] = -3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s10
			
			texldl r1, v2, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex10; w = 5, MipMapLODBias[10] = -4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s10
			
			texldl r1, v2, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex10; w = 5, MipMapLODBias[10] = -5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s10
			
			texldl r1, v2, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex10; w = 5, MipMapLODBias[10] = -6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s10
			
			texldl r1, v2, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, Tex10; w = 5, MipMapLODBias[10] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s10
			
			mov r0, v2
			texldl r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, Tex10; w = 5, MipMapLODBias[10] = -1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s10
			
			mov r1, v2
			texldl r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, Tex10; w = 5, MipMapLODBias[10] = -2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s10
			
			mov r2, v2
			texldl r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, Tex10; w = 5, MipMapLODBias[10] = -3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s10
			
			mov r3, v2
			texldl r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, Tex10; w = 5, MipMapLODBias[10] = -4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s10
			
			mov r4, v2
			texldl r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, Tex10; w = 5, MipMapLODBias[10] = -5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s10
			
			mov r5, v2
			texldl r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, Tex10; w = 5, MipMapLODBias[10] = -6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s10
			
			mov r6, v2
			texldl r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex15";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			texldl r0, v2, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2(.xyw), Tex15";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2.xyw
            dcl_2d s15
			
			texldl r0, v2, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, Tex15";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			mov r2, v2
			texldl r0, r2, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : c2, Tex15";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.0f, 0.0f, 0.0f, 2.f
            dcl_2d s15
			
			texldl r0, c2, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex15; writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			texldl r0.xyzw, v2, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, Tex15; writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			mov r2, v2
			texldl r0.xyzw, r2, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : c2, Tex15; writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.98f, 0.98f, 0.98f, 2.f
            dcl_2d s15
			
			texldl r0.xyzw, c2, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl_pp : v2, Tex15";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			texldl_pp r0, v2, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl_pp : r2, Tex15";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			mov r2, v2
			texldl_pp r0, r2, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl_pp : c2, Tex15";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.48f, 0.48f, 0.48f, 2.f
            dcl_2d s15
			
			texldl_pp r0, c2, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, Tex15; w = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, Tex15; w = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s15
			
			mov r1, v0.xy
			mov r1.w, c0.x
			texldl r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, Tex15; w = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_2d s15
			
			mov r2, v0.xy
			mov r2.w, c0.x
			texldl r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, Tex15; w = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_2d s15
			
			mov r3, v0.xy
			mov r3.w, c0.x
			texldl r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, Tex15; w = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_2d s15
			
			mov r4, v0.xy
			mov r4.w, c0.x
			texldl r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, Tex15; w = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s15
			
			mov r5, v0.xy
			mov r5.w, c0.x
			texldl r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, Tex15; w = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_2d s15
			
			mov r6, v0.xy
			mov r6.w, c0.x
			texldl r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex15; w = 0, MaxMipLevel[15] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			texldl r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex15; w = 0, MaxMipLevel[15] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 1;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			texldl r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex15; w = 0, MaxMipLevel[15] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 2;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			texldl r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex15; w = 0, MaxMipLevel[15] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			texldl r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex15; w = 0, MaxMipLevel[15] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 4;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			texldl r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex15; w = 0, MaxMipLevel[15] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 5;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			texldl r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex15; w = 0, MaxMipLevel[15] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 6;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			texldl r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, Tex15; w = 0, MaxMipLevel[15] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			mov r0, v0
			texldl r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, Tex15; w = 0, MaxMipLevel[15] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 1;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			mov r1, v0
			texldl r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, Tex15; w = 0, MaxMipLevel[15] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 2;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			mov r2, v0
			texldl r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, Tex15; w = 0, MaxMipLevel[15] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			mov r3, v0
			texldl r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, Tex15; w = 0, MaxMipLevel[15] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 4;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			mov r4, v0
			texldl r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, Tex15; w = 0, MaxMipLevel[15] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 5;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			mov r5, v0
			texldl r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, Tex15; w = 0, MaxMipLevel[15] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 6;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			mov r6, v0
			texldl r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex15; w = 0, MipMapLODBias[15] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			texldl r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex15; w = 0, MipMapLODBias[15] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			texldl r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex15; w = 0, MipMapLODBias[15] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			texldl r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex15; w = 0, MipMapLODBias[15] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			texldl r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex15; w = 0, MipMapLODBias[15] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			texldl r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex15; w = 0, MipMapLODBias[15] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			texldl r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, Tex15; w = 0, MipMapLODBias[15] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			texldl r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, Tex15; w = 0, MipMapLODBias[15] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			mov r0, v0
			texldl r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, Tex15; w = 0, MipMapLODBias[15] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			mov r1, v0
			texldl r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, Tex15; w = 0, MipMapLODBias[15] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			mov r2, v0
			texldl r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, Tex15; w = 0, MipMapLODBias[15] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			mov r3, v0
			texldl r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, Tex15; w = 0, MipMapLODBias[15] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			mov r4, v0
			texldl r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, Tex15; w = 0, MipMapLODBias[15] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			mov r5, v0
			texldl r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, Tex15; w = 0, MipMapLODBias[15] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s15
			
			mov r6, v0
			texldl r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex15; w = 5, MipMapLODBias[15] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			texldl r1, v2, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex15; w = 5, MipMapLODBias[15] = -1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			texldl r1, v2, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex15; w = 5, MipMapLODBias[15] = -2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			texldl r1, v2, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex15; w = 5, MipMapLODBias[15] = -3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			texldl r1, v2, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex15; w = 5, MipMapLODBias[15] = -4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			texldl r1, v2, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex15; w = 5, MipMapLODBias[15] = -5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			texldl r1, v2, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, Tex15; w = 5, MipMapLODBias[15] = -6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			texldl r1, v2, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, Tex15; w = 5, MipMapLODBias[15] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			mov r0, v2
			texldl r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, Tex15; w = 5, MipMapLODBias[15] = -1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			mov r1, v2
			texldl r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, Tex15; w = 5, MipMapLODBias[15] = -2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			mov r2, v2
			texldl r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, Tex15; w = 5, MipMapLODBias[15] = -3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			mov r3, v2
			texldl r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, Tex15; w = 5, MipMapLODBias[15] = -4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			mov r4, v2
			texldl r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, Tex15; w = 5, MipMapLODBias[15] = -5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			mov r5, v2
			texldl r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, Tex15; w = 5, MipMapLODBias[15] = -6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s15
			
			mov r6, v2
			texldl r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
