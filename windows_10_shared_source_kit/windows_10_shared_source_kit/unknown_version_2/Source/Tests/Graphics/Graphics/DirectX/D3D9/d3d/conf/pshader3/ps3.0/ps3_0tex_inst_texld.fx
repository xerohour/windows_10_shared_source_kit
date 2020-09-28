#include "include.fx"

Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0(.xy), Tex0";
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
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, Tex0";
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
			texld r0, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, Tex0";
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
			
			texld r0, c0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; writemask = xyzw";
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
			
			texld r0.xyzw, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, Tex0; writemask = xyzw";
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
			texld r0.xyzw, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, Tex0; writemask = xyzw";
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
			
			texld r0.xyzw, c0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v0, Tex0";
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
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			texld_pp r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r0, Tex0";
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
			texld_pp r0, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
	}
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c0, Tex0";
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
			
			texld_pp r0, c0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 0";
	String	Shape = "TexTL0";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 1";
	String	Shape = "TexTL1";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 2";
	String	Shape = "TexTL2";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 3";
	String	Shape = "TexTL3";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 4";
	String	Shape = "TexTL4";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 5";
	String	Shape = "TexTL5";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, Tex0; level = 0";
	String	Shape = "TexTL0";
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
			texld r0, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, Tex0; level = 1";
	String	Shape = "TexTL1";
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
			
			mov r1, v0
			texld r0, r1, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, Tex0; level = 2";
	String	Shape = "TexTL2";
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
			
			mov r2, v0
			texld r0, r2, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, Tex0; level = 3";
	String	Shape = "TexTL3";
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
			
			mov r3, v0
			texld r0, r3, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, Tex0; level = 4";
	String	Shape = "TexTL4";
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
			
			mov r4, v0
			texld r0, r4, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, Tex0; level = 5";
	String	Shape = "TexTL5";
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
			
			mov r5, v0
			texld r0, r5, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; MaxMipLevel[0] = 0";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; MaxMipLevel[0] = 1";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; MaxMipLevel[0] = 2";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; MaxMipLevel[0] = 3";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; MaxMipLevel[0] = 4";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; MaxMipLevel[0] = 5";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, Tex0; MaxMipLevel[0] = 0";
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
			texld r0, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, Tex0; MaxMipLevel[0] = 1";
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
			texld r0, r1, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, Tex0; MaxMipLevel[0] = 2";
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
			texld r0, r2, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, Tex0; MaxMipLevel[0] = 3";
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
			texld r0, r3, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, Tex0; MaxMipLevel[0] = 4";
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
			texld r0, r4, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, Tex0; MaxMipLevel[0] = 5";
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
			texld r0, r5, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 0, MipMapLODBias[0] = 0";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 0, MipMapLODBias[0] = 1";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 0, MipMapLODBias[0] = 2";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 0, MipMapLODBias[0] = 3";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 0, MipMapLODBias[0] = 4";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 0, MipMapLODBias[0] = 5";
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
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, Tex0; level = 0, MipMapLODBias[0] = 0";
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
			texld r0, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, Tex0; level = 0, MipMapLODBias[0] = 1";
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
			texld r0, r1, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, Tex0; level = 0, MipMapLODBias[0] = 2";
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
			texld r0, r2, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, Tex0; level = 0, MipMapLODBias[0] = 3";
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
			texld r0, r3, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, Tex0; level = 0, MipMapLODBias[0] = 4";
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
			texld r0, r4, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, Tex0; level = 0, MipMapLODBias[0] = 5";
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
			texld r0, r5, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 5, MipMapLODBias[0] = -0";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 5, MipMapLODBias[0] = -1";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 5, MipMapLODBias[0] = -2";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 5, MipMapLODBias[0] = -3";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 5, MipMapLODBias[0] = -4";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex0; level = 5, MipMapLODBias[0] = -5";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			texld r0, v0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r0, Tex0; level = 5, MipMapLODBias[0] = -0";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r0, v0
			texld r0, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r1, Tex0; level = 5, MipMapLODBias[0] = -1";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r1, v0
			texld r0, r1, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r2, Tex0; level = 5, MipMapLODBias[0] = -2";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r2, v0
			texld r0, r2, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r3, Tex0; level = 5, MipMapLODBias[0] = -3";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r3, v0
			texld r0, r3, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r4, Tex0; level = 5, MipMapLODBias[0] = -4";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r4, v0
			texld r0, r4, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r5, Tex0; level = 5, MipMapLODBias[0] = -5";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s0
			
			mov r5, v0
			texld r0, r5, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v1, Tex6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v1, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1(.xy), Tex6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1.xy
            dcl_2d s6
			
			texld r0, v1, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, Tex6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r1, v1
			texld r0, r1, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, Tex6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, c1, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1, Tex6; writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0.xyzw, v1, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, Tex6; writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r1, v1
			texld r0.xyzw, r1, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, Tex6; writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0.xyzw, c1, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v1, Tex6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1.xy
            dcl_2d s6
			
			texld_pp r0, v1, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r1, Tex6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r1, v1
			texld_pp r0, r1, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
	}
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c1, Tex6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld_pp r0, c1, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 1";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 2";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 3";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 4";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 5";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, Tex6; level = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r0, v0
			texld r0, r0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, Tex6; level = 1";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r1, v0
			texld r0, r1, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, Tex6; level = 2";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r2, v0
			texld r0, r2, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, Tex6; level = 3";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r3, v0
			texld r0, r3, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, Tex6; level = 4";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r4, v0
			texld r0, r4, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, Tex6; level = 5";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r5, v0
			texld r0, r5, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; MaxMipLevel[6] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; MaxMipLevel[6] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; MaxMipLevel[6] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; MaxMipLevel[6] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; MaxMipLevel[6] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; MaxMipLevel[6] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, Tex6; MaxMipLevel[6] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r0, v0
			texld r0, r0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, Tex6; MaxMipLevel[6] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r1, v0
			texld r0, r1, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, Tex6; MaxMipLevel[6] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r2, v0
			texld r0, r2, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, Tex6; MaxMipLevel[6] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r3, v0
			texld r0, r3, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, Tex6; MaxMipLevel[6] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r4, v0
			texld r0, r4, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, Tex6; MaxMipLevel[6] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r5, v0
			texld r0, r5, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 0, MipMapLODBias[6] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 0, MipMapLODBias[6] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 0, MipMapLODBias[6] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 0, MipMapLODBias[6] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 0, MipMapLODBias[6] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 0, MipMapLODBias[6] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, Tex6; level = 0, MipMapLODBias[6] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r0, v0
			texld r0, r0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, Tex6; level = 0, MipMapLODBias[6] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r1, v0
			texld r0, r1, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, Tex6; level = 0, MipMapLODBias[6] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r2, v0
			texld r0, r2, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, Tex6; level = 0, MipMapLODBias[6] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r3, v0
			texld r0, r3, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, Tex6; level = 0, MipMapLODBias[6] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r4, v0
			texld r0, r4, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, Tex6; level = 0, MipMapLODBias[6] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
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
            dcl_2d s6
			
			mov r5, v0
			texld r0, r5, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 5, MipMapLODBias[6] = -0";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_2>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 5, MipMapLODBias[6] = -1";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_2>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 5, MipMapLODBias[6] = -2";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_2>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 5, MipMapLODBias[6] = -3";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_2>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 5, MipMapLODBias[6] = -4";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_2>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex6; level = 5, MipMapLODBias[6] = -5";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_2>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s6
			
			texld r0, v0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r0, Tex6; level = 5, MipMapLODBias[6] = -0";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_2>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s6
			
			mov r0, v0
			texld r0, r0, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r1, Tex6; level = 5, MipMapLODBias[6] = -1";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_2>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s6
			
			mov r1, v0
			texld r0, r1, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r2, Tex6; level = 5, MipMapLODBias[6] = -2";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_2>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s6
			
			mov r2, v0
			texld r0, r2, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r3, Tex6; level = 5, MipMapLODBias[6] = -3";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_2>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s6
			
			mov r3, v0
			texld r0, r3, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r4, Tex6; level = 5, MipMapLODBias[6] = -4";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_2>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s6
			
			mov r4, v0
			texld r0, r4, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r5, Tex6; level = 5, MipMapLODBias[6] = -5";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_2>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s6
			
			mov r5, v0
			texld r0, r5, s6
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v2, Tex11";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v2, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2(.xy), Tex11";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2.xy
            dcl_2d s11
			
			texld r0, v2, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, Tex11";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r2, v2
			texld r0, r2, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, Tex11";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, c2, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, Tex11; writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0.xyzw, v2, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, Tex11; writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r2, v2
			texld r0.xyzw, r2, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, Tex11; writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0.xyzw, c2, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v2, Tex11";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2.xy
            dcl_2d s11
			
			texld_pp r0, v2, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r2, Tex11";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r2, v2
			texld_pp r0, r2, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
	}
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c2, Tex11";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld_pp r0, c2, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 1";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 2";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 3";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 4";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 5";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, Tex11; level = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r0, v0
			texld r0, r0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, Tex11; level = 1";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r1, v0
			texld r0, r1, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, Tex11; level = 2";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r2, v0
			texld r0, r2, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, Tex11; level = 3";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r3, v0
			texld r0, r3, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, Tex11; level = 4";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r4, v0
			texld r0, r4, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, Tex11; level = 5";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r5, v0
			texld r0, r5, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; MaxMipLevel[11] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; MaxMipLevel[11] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; MaxMipLevel[11] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; MaxMipLevel[11] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; MaxMipLevel[11] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; MaxMipLevel[11] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, Tex11; MaxMipLevel[11] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r0, v0
			texld r0, r0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, Tex11; MaxMipLevel[11] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r1, v0
			texld r0, r1, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, Tex11; MaxMipLevel[11] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r2, v0
			texld r0, r2, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, Tex11; MaxMipLevel[11] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r3, v0
			texld r0, r3, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, Tex11; MaxMipLevel[11] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r4, v0
			texld r0, r4, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, Tex11; MaxMipLevel[11] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r5, v0
			texld r0, r5, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 0, MipMapLODBias[11] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 0, MipMapLODBias[11] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 0, MipMapLODBias[11] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 0, MipMapLODBias[11] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 0, MipMapLODBias[11] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 0, MipMapLODBias[11] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, Tex11; level = 0, MipMapLODBias[11] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r0, v0
			texld r0, r0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, Tex11; level = 0, MipMapLODBias[11] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r1, v0
			texld r0, r1, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, Tex11; level = 0, MipMapLODBias[11] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r2, v0
			texld r0, r2, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, Tex11; level = 0, MipMapLODBias[11] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r3, v0
			texld r0, r3, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, Tex11; level = 0, MipMapLODBias[11] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r4, v0
			texld r0, r4, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, Tex11; level = 0, MipMapLODBias[11] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
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
            dcl_2d s11
			
			mov r5, v0
			texld r0, r5, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 5, MipMapLODBias[11] = -0";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_2>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 5, MipMapLODBias[11] = -1";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_2>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 5, MipMapLODBias[11] = -2";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_2>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 5, MipMapLODBias[11] = -3";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_2>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 5, MipMapLODBias[11] = -4";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_2>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, Tex11; level = 5, MipMapLODBias[11] = -5";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_2>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s11
			
			texld r0, v0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r0, Tex11; level = 5, MipMapLODBias[11] = -0";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_2>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s11
			
			mov r0, v0
			texld r0, r0, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r1, Tex11; level = 5, MipMapLODBias[11] = -1";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_2>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s11
			
			mov r1, v0
			texld r0, r1, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r2, Tex11; level = 5, MipMapLODBias[11] = -2";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_2>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s11
			
			mov r2, v0
			texld r0, r2, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r3, Tex11; level = 5, MipMapLODBias[11] = -3";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_2>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s11
			
			mov r3, v0
			texld r0, r3, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r4, Tex11; level = 5, MipMapLODBias[11] = -4";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_2>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s11
			
			mov r4, v0
			texld r0, r4, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r5, Tex11; level = 5, MipMapLODBias[11] = -5";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_2>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_2d s11
			
			mov r5, v0
			texld r0, r5, s11
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}