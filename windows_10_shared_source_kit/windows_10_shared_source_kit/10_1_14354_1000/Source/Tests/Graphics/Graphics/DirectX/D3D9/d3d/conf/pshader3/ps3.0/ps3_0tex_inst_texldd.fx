#include "include.fx"

Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx(c2)=(0.04, 0, 0, 0), dsy(c2.w)=(0, 0, 0, 0) -> level = 0";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.04, 0.0, 0.0, 0.0
            dcl_2d s0
			
			texldd	r0, v0, s0, c2, c2.wwww
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0(.xy), Tex0, dsx(c2)=(0, 0.07, 0, 0), dsy(c2.w)=(0, 0, 0, 0) -> level = 1";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c2, 0.0, 0.07, 0.0, 0.0
            dcl_2d s0
			
			texldd r0, v0, s0, c2, c2.wwww
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx(c2)=(0.09192, 0.09192, 0, 0), dsy(c2.w)=(0, 0, 0, 0) -> level = 2";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.09192, 0.09192, 0.0, 0.0		// 0.09192 = 0.13 / sqrt(2)
            dcl_2d s0
			
			mov r0, v0
			texldd r0, r0, s0, c2, c2.wwww
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx(r0.x)=(0, 0, 0, 0), dsy(r1)=(0.3, 0, 1, 0) -> level = 3";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.3, 0.0, 1.0, 0.0
            dcl_2d s0
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			mov r1, c2
			texldd r0, v0, s0, r0.xxxx, r1
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx = v0, dsy = 0 -> level = NA";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 0.6, 1.0, 0.0 // level = 4
            dcl_2d s0
			
			texldd r0, v0, s0, v0, c2.wwww
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=v0, dsy(r1)=(0.7778, 0.7778, 1, 0) -> level = NA(5)";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			def  c2, 0.7778, 0.7778, 1.0, 0.0	// 0.7778 = 1.1 / sqrt(2)
            dcl_2d s0
			
			mov r0, v0
			mov r1, c2
			texldd r0, r0, s0, v1, r1
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : c2, Tex0, dsx=v0, dsy(c2)=(0, 0.04, 0, 1) -> level = NA(0)";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 0.04, 0.0, 1.0
            dcl_2d s0
			
			texldd r0, c2, s0, v0, c2
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx(c2.z)=(0, 0, 0, 0), dsy=v1 -> level = NA(1)";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			def  c2, 0.232535, 0.232535, 0.0, 1.0	// 0.232535=.07/sqrt(2)
            dcl_2d s0
			
			mov r0, v0
			mov r1, c2
			texldd r2, r0, s0, c2.zzzz, v1
			mov oC0, r2
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx(r0)=(0.13, 0, 0, 1), dsy(c2)=(0, 0.13, 0, 1) -> level = 2";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 0.13, 0.0, 1.0
            dcl_2d s0
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			texldd r2, v0, s0, r0, c2
			mov oC0, r2
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : c2, Tex0, dsx(c2)=(0, 0.3, 1, 1), dsy=v0 -> level = NA(3)";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 0.3, 1.0, 1.0
            dcl_2d s0
			
			texldd r0, c2, s0, c2, v0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd_pp : v0, Tex0, dsx=(1), dsy=(0.424264, 0.424264, 1, 1) -> level = 4";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.424264, 0.424264, 1.0, 1.0	// 0.424264 = 0.6/sqrt(2)
            dcl_2d s0
			
			texldd_pp r0, v0, s0, c2.wwww, c2
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd_pp : r0, Tex0, dsx=(1.1, 0, 1, 1), dsy=(1.1, 0, 1, 1) -> level = 5";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 1.1, 0.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0, v0
			texldd_pp r0, r0, s0, c2, c2
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, repeat = 0.5, dsx=0/w+1/4w, dsy=1/h+1/4h => level=0";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0.f, 1.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, repeat = 0.5, dsx=2/w+1/4w, dsy=1/h+1/4h => level=1";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.f, 1.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, repeat = 0.5, dsx=2/w+1/4w, dsy=4/h+1/4h => level=2";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.f, 4.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, repeat = 0.5, dsx=8/w+1/4w, dsy=4/h+1/4h => level=3";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 8.f, 4.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, repeat = 0.5, dsx=8/w+1/4w, dsy=16/h+1/4h => level=4";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 8.f, 16.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, repeat = 0.5, dsx=32/w+1/4w, dsy=16/h+1/4h => level=5";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 32.f, 16.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/2w, dsy=1/2h, level=0, writemask=xyzw";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/2w, dsy=1/2h, level=1, writemask=xyzw";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/2w, dsy=1/2h, level=2, writemask=xyzw";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/2w, dsy=1/2h, level=3, writemask=xyzw";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/2w, dsy=1/2h, level=4, writemask=xyzw";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/2w, dsy=1/2h, level=5, writemask=xyzw";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/2w, dsy=1/2h, level=0, writemask=xyzw";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/2w, dsy=1/2h, level=1, writemask=xyzw";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/2w, dsy=1/2h, level=2, writemask=xyzw";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/2w, dsy=1/2h, level=3, writemask=xyzw";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/2w, dsy=1/2h, level=4, writemask=xyzw";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/2w, dsy=1/2h, level=5, writemask=xyzw";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/2w, dsy=1/2h, MaxMipLevel[0] = 0 => level=0";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/2w, dsy=1/2h, MaxMipLevel[0] = 1 => level=1";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/2w, dsy=1/2h, MaxMipLevel[0] = 2 => level=2";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/2w, dsy=1/2h, MaxMipLevel[0] = 3 => level=3";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/2w, dsy=1/2h, MaxMipLevel[0] = 4 => level=4";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/2w, dsy=1/2h, MaxMipLevel[0] = 5 => level=5";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/2w, dsy=1/2h, MaxMipLevel[0] = 0 => level=0";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/2w, dsy=1/2h, MaxMipLevel[0] = 1 => level=1";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/2w, dsy=1/2h, MaxMipLevel[0] = 2 => level=2";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/2w, dsy=1/2h, MaxMipLevel[0] = 3 => level=3";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/2w, dsy=1/2h, MaxMipLevel[0] = 4 => level=4";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/2w, dsy=1/2h, MaxMipLevel[0] = 5 => level=5";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/w, dsy=1/h, MipMapLODBias[0] = 0 => level=0";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/w, dsy=1/h, MipMapLODBias[0] = 1 => level=1";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/w, dsy=1/h, MipMapLODBias[0] = 2 => level=2";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/w, dsy=1/h, MipMapLODBias[0] = 3 => level=3";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/w, dsy=1/h, MipMapLODBias[0] = 4 => level=4";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1/w, dsy=1/h, MipMapLODBias[0] = 5 => level=5";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/w, dsy=1/h, MipMapLODBias[0] = 0 => level=0";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/w, dsy=1/h, MipMapLODBias[0] = 1 => level=1";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/w, dsy=1/h, MipMapLODBias[0] = 2 => level=2";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/w, dsy=1/h, MipMapLODBias[0] = 3 => level=3";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/w, dsy=1/h, MipMapLODBias[0] = 4 => level=4";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1/w, dsy=1/h, MipMapLODBias[0] = 5 => level=5";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1, dsy=1, MipMapLODBias[0] = -0  => level=5-0";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s0
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1, dsy=1, MipMapLODBias[0] = -1  => level=5-1";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s0
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1, dsy=1, MipMapLODBias[0] = -2  => level=5-2";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s0
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1, dsy=1, MipMapLODBias[0] = -3  => level=5-3";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s0
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1, dsy=1, MipMapLODBias[0] = -4  => level=5-4";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s0
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex0, dsx=1, dsy=1, MipMapLODBias[0] = -5  => level=5-5";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s0
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1, dsy=1, MipMapLODBias[0] = -0  => level=5-0";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1, dsy=1, MipMapLODBias[0] = -1  => level=5-1";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1, dsy=1, MipMapLODBias[0] = -2  => level=5-2";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1, dsy=1, MipMapLODBias[0] = -3  => level=5-3";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1, dsy=1, MipMapLODBias[0] = -4  => level=5-4";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex0, dsx=1, dsy=1, MipMapLODBias[0] = -5  => level=5-5";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s0, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v1, Tex8, dsx(c2)=(0.04, 0, 0, 0), dsy(c2.w)=(0, 0, 0, 0) -> level = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.04, 0.0, 0.0, 0.0
            dcl_2d s8
			
			texldd	r0, v1, s8, c2, c2.wwww
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v1(.xy), Tex8, dsx(c2.w)=(0, 0, 0, 0), dsy(c2)=(0, 0.07, 0, 0) -> level = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1.xy
			def  c2, 0.0, 0.07, 0.0, 0.0
            dcl_2d s8
			
			texldd r0, v1, s8, c2.wwww, c2
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r1, Tex8, dsx(c2.w)=(0, 0, 0, 0), dsy(c2)=(0.09192, 0.09192, 0, 0) -> level = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.09192, 0.09192, 0.0, 0.0		// 0.09192 = 0.13 / sqrt(2)
            dcl_2d s8
			
			mov r1, v1
			texldd r0, r1, s8, c2.wwww, c2
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v1, Tex8, dsx(r0)=(0, 0.3, 1, 0), dsy(r1)=(0.3, 0, 1, 0) -> level = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.3, 0.0, 1.0, 0.0
            dcl_2d s8
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			mov r1, c2
			texldd r0, v1, s8, r0, r1
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v1, Tex8, dsx = 0, dsy = v1 -> level = NA";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.0, 0.6, 1.0, 0.0 // level = 4
            dcl_2d s8
			
			texldd r0, v1, s8, c2.wwww, v1
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=v1, dsy(r1)=(0.7778, 0.7778, 1, 0) -> level = NA(5)";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			def  c2, 0.7778, 0.7778, 1.0, 0.0	// 0.7778 = 1.1 / sqrt(2)
            dcl_2d s8
			
			mov r0, v0
			mov r1, c2
			texldd r0, r0, s8, v1, r1
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : c2, Tex8, dsx=v1, dsy(c2)=(0, 0.04, 0, 1) -> level = NA(0)";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.0, 0.04, 0.0, 1.0
            dcl_2d s8
			
			texldd r0, c2, s8, v1, c2
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx(c2.z)=(0, 0, 0, 0), dsy=v1 -> level = NA(1)";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			def  c2, 0.232535, 0.232535, 0.0, 1.0	// 0.232535=.07/sqrt(2)
            dcl_2d s8
			
			mov r0, v0
			mov r1, c2
			texldd r2, r0, s8, c2.zzzz, v1
			mov oC0, r2
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v1, Tex8, dsx(r0)=(0.13, 0, 0, 1), dsy(c2)=(0, 0.13, 0, 1) -> level = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.0, 0.13, 0.0, 1.0
            dcl_2d s8
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			texldd r2, v1, s8, r0, c2
			mov oC0, r2
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : c2, Tex8, dsx(c2)=(0, 0.3, 1, 1), dsy=v1 -> level = NA(3)";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.0, 0.3, 1.0, 1.0
            dcl_2d s8
			
			texldd r0, c2, s8, c2, v1
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd_pp : v1, Tex8, dsx=(0.424264, 0.424264, 1, 1), dsy=(1, 1, 1, 1) -> level = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.424264, 0.424264, 1.0, 1.0	// 0.424264 = 0.6/sqrt(2)
            dcl_2d s8
			
			texldd_pp r0, v1, s8, c2, c2.wwww
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd_pp : r1, Tex8, dsx=(1.1, 0, 1, 1), dsy=(1, 1, 1, 1) -> level = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 1.1, 0.0, 1.0, 1.0
            dcl_2d s8
			
			mov r1, v1
			texldd_pp r0, r1, s8, c2, c2.wwww
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, repeat = 1.5, dsx=0/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0.f, 1.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, repeat = 1.5, dsx=2/w+1/4w, dsy=1/h+1/4h => level=1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.f, 1.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, repeat = 1.5, dsx=2/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.f, 4.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, repeat = 1.5, dsx=8/w+1/4w, dsy=4/h+1/4h => level=3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 8.f, 4.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, repeat = 1.5, dsx=8/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 8.f, 16.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, repeat = 1.5, dsx=32/w+1/4w, dsy=16/h+1/4h => level=5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 32.f, 16.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/2w, dsy=1/2h, level=0, writemask=xyzw";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/2w, dsy=1/2h, level=1, writemask=xyzw";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/2w, dsy=1/2h, level=2, writemask=xyzw";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/2w, dsy=1/2h, level=3, writemask=xyzw";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/2w, dsy=1/2h, level=4, writemask=xyzw";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/2w, dsy=1/2h, level=5, writemask=xyzw";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/2w, dsy=1/2h, level=0, writemask=xyzw";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/2w, dsy=1/2h, level=1, writemask=xyzw";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/2w, dsy=1/2h, level=2, writemask=xyzw";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/2w, dsy=1/2h, level=3, writemask=xyzw";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/2w, dsy=1/2h, level=4, writemask=xyzw";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/2w, dsy=1/2h, level=5, writemask=xyzw";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/2w, dsy=1/2h, MaxMipLevel[8] = 0 => level=0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/2w, dsy=1/2h, MaxMipLevel[8] = 1 => level=1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 1;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/2w, dsy=1/2h, MaxMipLevel[8] = 2 => level=2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/2w, dsy=1/2h, MaxMipLevel[8] = 3 => level=3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 3;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/2w, dsy=1/2h, MaxMipLevel[8] = 4 => level=4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 4;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/2w, dsy=1/2h, MaxMipLevel[8] = 5 => level=5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 5;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/2w, dsy=1/2h, MaxMipLevel[8] = 0 => level=0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/2w, dsy=1/2h, MaxMipLevel[8] = 1 => level=1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 1;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/2w, dsy=1/2h, MaxMipLevel[8] = 2 => level=2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/2w, dsy=1/2h, MaxMipLevel[8] = 3 => level=3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 3;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/2w, dsy=1/2h, MaxMipLevel[8] = 4 => level=4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 4;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/2w, dsy=1/2h, MaxMipLevel[8] = 5 => level=5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 5;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/w, dsy=1/h, MipMapLODBias[8] = 0 => level=0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/w, dsy=1/h, MipMapLODBias[8] = 1 => level=1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/w, dsy=1/h, MipMapLODBias[8] = 2 => level=2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/w, dsy=1/h, MipMapLODBias[8] = 3 => level=3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/w, dsy=1/h, MipMapLODBias[8] = 4 => level=4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1/w, dsy=1/h, MipMapLODBias[8] = 5 => level=5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/w, dsy=1/h, MipMapLODBias[8] = 0 => level=0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/w, dsy=1/h, MipMapLODBias[8] = 1 => level=1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/w, dsy=1/h, MipMapLODBias[8] = 2 => level=2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/w, dsy=1/h, MipMapLODBias[8] = 3 => level=3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/w, dsy=1/h, MipMapLODBias[8] = 4 => level=4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1/w, dsy=1/h, MipMapLODBias[8] = 5 => level=5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s8
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1, dsy=1, MipMapLODBias[8] = -0  => level=5-0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s8
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1, dsy=1, MipMapLODBias[8] = -1  => level=5-1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s8
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1, dsy=1, MipMapLODBias[8] = -2  => level=5-2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s8
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1, dsy=1, MipMapLODBias[8] = -3  => level=5-3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s8
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1, dsy=1, MipMapLODBias[8] = -4  => level=5-4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s8
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex8, dsx=1, dsy=1, MipMapLODBias[8] = -5  => level=5-5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s8
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1, dsy=1, MipMapLODBias[8] = -0  => level=5-0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s8
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1, dsy=1, MipMapLODBias[8] = -1  => level=5-1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s8
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1, dsy=1, MipMapLODBias[8] = -2  => level=5-2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s8
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1, dsy=1, MipMapLODBias[8] = -3  => level=5-3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s8
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1, dsy=1, MipMapLODBias[8] = -4  => level=5-4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s8
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex8, dsx=1, dsy=1, MipMapLODBias[8] = -5  => level=5-5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s8
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s8, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v2, Tex13, dsx(c2.wwww)=(0, 0, 0, 0), dsy(c2)=(0.04, 0, 0, 0) -> level = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.04, 0.0, 0.0, 0.0
            dcl_2d s13
			
			texldd	r0, v2, s13, c2.w, c2
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v2(.xy), Tex13, dsx(c2)=(0, 0.07, 0, 0), dsy(c2.w)=(0, 0, 0, 0) -> level = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2.xy
			def  c2, 0.0, 0.07, 0.0, 0.0
            dcl_2d s13
			
			texldd r0, v2, s13, c2, c2.w
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r2, Tex13, dsx(c2.w)=(0, 0, 0, 0), dsy(c2)=(0.09192, 0.09192, 0, 0) -> level = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.09192, 0.09192, 0.0, 0.0		// 0.09192 = 0.13 / sqrt(2)
            dcl_2d s13
			
			mov r2, v2
			texldd r0, r2, s13, c2.wwww, c2
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v2, Tex13, dsx(r0)=(0, 0.3, 1, 0), dsy(r1)=(0.3, 0, 1, 0) -> level = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.3, 0.0, 1.0, 0.0
            dcl_2d s13
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			mov r1, c2
			texldd r0, v2, s13, r0, r1
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v2, Tex13, dsx = 0, dsy = v2 -> level = NA";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.0, 0.6, 1.0, 0.0 // level = 4
            dcl_2d s13
			
			texldd r0, v2, s13, c2.wwww, v2
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=v2, dsy(r1)=(0.7778, 0.7778, 1, 0) -> level = NA(5)";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			def  c2, 0.7778, 0.7778, 1.0, 0.0	// 0.7778 = 1.1 / sqrt(2)
            dcl_2d s13
			
			mov r0, v0
			mov r1, c2
			texldd r0, r0, s13, v1, r1
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : c2, Tex13, dsx=v2, dsy(c2)=(0, 0.04, 0, 1) -> level = NA(0)";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.0, 0.04, 0.0, 1.0
            dcl_2d s13
			
			texldd r0, c2, s13, v2, c2
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx(c2.z)=(0, 0, 0, 0), dsy=v1 -> level = NA(1)";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			def  c2, 0.232535, 0.232535, 0.0, 1.0	// 0.232535=.07/sqrt(2)
            dcl_2d s13
			
			mov r0, v0
			mov r1, c2
			texldd r2, r0, s13, c2.zzzz, v1
			mov oC0, r2
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v2, Tex13, dsx(r0)=(0.13, 0, 0, 1), dsy(c2)=(0, 0.13, 0, 1) -> level = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.0, 0.13, 0.0, 1.0
            dcl_2d s13
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			texldd r2, v2, s13, r0, c2
			mov oC0, r2
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : c2, Tex13, dsx(c2)=(0, 0.3, 1, 1), dsy=v2 -> level = NA(3)";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.0, 0.3, 1.0, 1.0
            dcl_2d s13
			
			texldd r0, c2, s13, c2, v2
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd_pp : v2, Tex13, dsx=(0.424264, 0.424264, 1, 1), dsy=(1, 1, 1, 1) -> level = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.424264, 0.424264, 1.0, 1.0	// 0.424264 = 0.6/sqrt(2)
            dcl_2d s13
			
			texldd_pp r0, v2, s13, c2, c2.wwww
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd_pp : r2, Tex13, dsx=(1.1, 0, 1, 1), dsy=(1.1, 0, 1, 1) -> level = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 1.1, 0.0, 1.0, 1.0
            dcl_2d s13
			
			mov r2, v2
			texldd_pp r0, r2, s13, c2, c2
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, repeat = 2.5, dsx=0/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0.f, 1.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, repeat = 2.5, dsx=2/w+1/4w, dsy=1/h+1/4h => level=1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.f, 1.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, repeat = 2.5, dsx=2/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.f, 4.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, repeat = 2.5, dsx=8/w+1/4w, dsy=4/h+1/4h => level=3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 8.f, 4.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, repeat = 2.5, dsx=8/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 8.f, 16.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, repeat = 2.5, dsx=32/w+1/4w, dsy=16/h+1/4h => level=5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 32.f, 16.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/2w, dsy=1/2h, level=0, writemask=xyzw";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/2w, dsy=1/2h, level=1, writemask=xyzw";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/2w, dsy=1/2h, level=2, writemask=xyzw";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/2w, dsy=1/2h, level=3, writemask=xyzw";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/2w, dsy=1/2h, level=4, writemask=xyzw";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/2w, dsy=1/2h, level=5, writemask=xyzw";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/2w, dsy=1/2h, level=0, writemask=xyzw";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/2w, dsy=1/2h, level=1, writemask=xyzw";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/2w, dsy=1/2h, level=2, writemask=xyzw";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/2w, dsy=1/2h, level=3, writemask=xyzw";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/2w, dsy=1/2h, level=4, writemask=xyzw";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/2w, dsy=1/2h, level=5, writemask=xyzw";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/2w, dsy=1/2h, MaxMipLevel[13] = 0 => level=0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/2w, dsy=1/2h, MaxMipLevel[13] = 1 => level=1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/2w, dsy=1/2h, MaxMipLevel[13] = 2 => level=2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 2;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/2w, dsy=1/2h, MaxMipLevel[13] = 3 => level=3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 3;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/2w, dsy=1/2h, MaxMipLevel[13] = 4 => level=4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 4;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/2w, dsy=1/2h, MaxMipLevel[13] = 5 => level=5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 5;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/2w, dsy=1/2h, MaxMipLevel[13] = 0 => level=0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/2w, dsy=1/2h, MaxMipLevel[13] = 1 => level=1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/2w, dsy=1/2h, MaxMipLevel[13] = 2 => level=2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 2;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/2w, dsy=1/2h, MaxMipLevel[13] = 3 => level=3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 3;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/2w, dsy=1/2h, MaxMipLevel[13] = 4 => level=4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 4;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/2w, dsy=1/2h, MaxMipLevel[13] = 5 => level=5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 5;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/w, dsy=1/h, MipMapLODBias[13] = 0 => level=0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/w, dsy=1/h, MipMapLODBias[13] = 1 => level=1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/w, dsy=1/h, MipMapLODBias[13] = 2 => level=2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/w, dsy=1/h, MipMapLODBias[13] = 3 => level=3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/w, dsy=1/h, MipMapLODBias[13] = 4 => level=4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1/w, dsy=1/h, MipMapLODBias[13] = 5 => level=5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/w, dsy=1/h, MipMapLODBias[13] = 0 => level=0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/w, dsy=1/h, MipMapLODBias[13] = 1 => level=1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/w, dsy=1/h, MipMapLODBias[13] = 2 => level=2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/w, dsy=1/h, MipMapLODBias[13] = 3 => level=3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/w, dsy=1/h, MipMapLODBias[13] = 4 => level=4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1/w, dsy=1/h, MipMapLODBias[13] = 5 => level=5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 1.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_2d s13
			
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
			mad r2.y, c0.x, r1.x, r1.z
			mad r3.x, c0.y, r1.y, r1.w
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1, dsy=1, MipMapLODBias[13] = -0  => level=5-0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s13
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1, dsy=1, MipMapLODBias[13] = -1  => level=5-1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s13
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1, dsy=1, MipMapLODBias[13] = -2  => level=5-2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s13
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1, dsy=1, MipMapLODBias[13] = -3  => level=5-3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s13
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1, dsy=1, MipMapLODBias[13] = -4  => level=5-4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s13
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, Tex13, dsx=1, dsy=1, MipMapLODBias[13] = -5  => level=5-5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s13
			
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, v0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1, dsy=1, MipMapLODBias[13] = -0  => level=5-0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s13
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1, dsy=1, MipMapLODBias[13] = -1  => level=5-1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s13
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1, dsy=1, MipMapLODBias[13] = -2  => level=5-2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s13
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1, dsy=1, MipMapLODBias[13] = -3  => level=5-3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s13
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1, dsy=1, MipMapLODBias[13] = -4  => level=5-4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s13
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, Tex13, dsx=1, dsy=1, MipMapLODBias[13] = -5  => level=5-5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 1.0, 0.0, 0.0
            dcl_2d s13
			
			mov r0, v0
			mov r2, c2.x
			mov r3, c2.x
			mov r2.x, c2.y
			mov r3.y, c2.y
			texldd r4, r0, s13, r2, r3
			mov oC0, r4
        };
		VertexShader = <TexTLVS>;
   }
}
