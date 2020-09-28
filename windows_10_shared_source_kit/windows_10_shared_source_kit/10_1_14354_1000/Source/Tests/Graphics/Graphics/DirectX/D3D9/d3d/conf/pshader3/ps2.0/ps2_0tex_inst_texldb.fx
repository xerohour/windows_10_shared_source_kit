#include "include.fx"

Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0";
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
			ps_2_0
			dcl t0.xy
			def  c0, 0.0, 0.0, 0.0, 0.0
                        dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 1";
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
			ps_2_0
			dcl t0.xy
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 2";
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
			ps_2_0
			dcl t0.xy
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 3";
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
			ps_2_0
			dcl t0.xy
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, level = 5, w = -0";
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
			ps_2_0
			dcl t0.xy
			def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, level = 5, w = -1";
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
			ps_2_0
			dcl t0.xy
			def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, level = 5, w = -2";
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
			ps_2_0
			dcl t0.xy
			def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, level = 5, w = -3";
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
			ps_2_0
			dcl t0.xy
			def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xy
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xy
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, level = 5, w = -0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xy
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, level = 5, w = -1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xy
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, level = 5, w = -2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xy
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, level = 5, w = -3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xy
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0, MaxMipLevel[0] = 0";
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0, MaxMipLevel[0] = 1";
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0, MaxMipLevel[0] = 2";
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0, MaxMipLevel[0] = 3";
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0, MaxMipLevel[0] = 4";
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0, MaxMipLevel[0] = 5";
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0, MaxMipLevel[0] = 6";
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0, MipMapLODBias[0] = 0";
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0, MipMapLODBias[0] = 1";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0, MipMapLODBias[0] = 2";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0, MipMapLODBias[0] = 3";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0, MipMapLODBias[0] = 4";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0, MipMapLODBias[0] = 5";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 0, MipMapLODBias[0] = 6";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 5, MipMapLODBias[0] = -0";
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
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 5, MipMapLODBias[0] = -1";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 5, MipMapLODBias[0] = -2";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 5, MipMapLODBias[0] = -3";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 5, MipMapLODBias[0] = -4";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 5, MipMapLODBias[0] = -5";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex0, w = 5, MipMapLODBias[0] = -6";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, level = 5, w = -0";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, level = 5, w = -1";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, level = 5, w = -2";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, level = 5, w = -3";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0, writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 1, writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 2, writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 3, writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, level = 5, w = -0, writemask=xyzw";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, level = 5, w = -1, writemask=xyzw";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, level = 5, w = -2, writemask=xyzw";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, level = 5, w = -3, writemask=xyzw";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0, MaxMipLevel[7] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0, MaxMipLevel[7] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0, MaxMipLevel[7] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0, MaxMipLevel[7] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0, MaxMipLevel[7] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0, MaxMipLevel[7] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0, MaxMipLevel[7] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0, MipMapLODBias[7] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0, MipMapLODBias[7] = 1";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0, MipMapLODBias[7] = 2";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0, MipMapLODBias[7] = 3";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0, MipMapLODBias[7] = 4";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0, MipMapLODBias[7] = 5";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 0, MipMapLODBias[7] = 6";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 5, MipMapLODBias[7] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 5, MipMapLODBias[7] = -1";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 5, MipMapLODBias[7] = -2";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 5, MipMapLODBias[7] = -3";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 5, MipMapLODBias[7] = -4";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 5, MipMapLODBias[7] = -5";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex7, w = 5, MipMapLODBias[7] = -6";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s7
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, level = 5, w = -0";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, level = 5, w = -1";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, level = 5, w = -2";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, level = 5, w = -3";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0, writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 1, writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 2, writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 3, writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, level = 5, w = -0, writemask=xyzw";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, level = 5, w = -1, writemask=xyzw";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, level = 5, w = -2, writemask=xyzw";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, level = 5, w = -3, writemask=xyzw";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0, MaxMipLevel[12] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0, MaxMipLevel[12] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0, MaxMipLevel[12] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0, MaxMipLevel[12] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0, MaxMipLevel[12] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0, MaxMipLevel[12] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0, MaxMipLevel[12] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0, MipMapLODBias[12] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0, MipMapLODBias[12] = 1";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0, MipMapLODBias[12] = 2";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0, MipMapLODBias[12] = 3";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0, MipMapLODBias[12] = 4";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0, MipMapLODBias[12] = 5";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 0, MipMapLODBias[12] = 6";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 5, MipMapLODBias[12] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 5, MipMapLODBias[12] = -1";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 5, MipMapLODBias[12] = -2";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 5, MipMapLODBias[12] = -3";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 5, MipMapLODBias[12] = -4";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 5, MipMapLODBias[12] = -5";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : Tex12, w = 5, MipMapLODBias[12] = -6";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_2d s12
			
			mov r0.x, t0.x  mov r0.y, t0.y 
			mov r0.w, c0.x
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
