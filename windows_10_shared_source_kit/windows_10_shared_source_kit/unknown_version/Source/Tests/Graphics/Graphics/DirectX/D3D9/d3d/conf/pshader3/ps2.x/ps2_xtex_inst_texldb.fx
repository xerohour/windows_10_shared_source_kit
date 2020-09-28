#include "include.fx"

//need to change all cases with w==5 to w==3
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex0";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r0, t0
			texldb r0, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0; writemask=xyzw";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r1.xyzw, t0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex0; writemask=xyzw";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r0, t0
			texldb r0.xyzw, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : t0, Tex0";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb_pp r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : r0, Tex0";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r0, t0
			texldb_pp r0, r0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex0; w = 0";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex0; w = 1";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 1.0
            dcl_2d s0
			
			mov r1, c0
			mov r1.x, t0.x
			mov r1.y, t0.y
			texldb r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, Tex0; w = 2";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 2.0
            dcl_2d s0
			
			mov r2, c0
			mov r2.x, t0.x
			mov r2.y, t0.y
			texldb r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, Tex0; w = 3";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 3.0
            dcl_2d s0
			
			mov r3, c0
			mov r3.x, t0.x
			mov r3.y, t0.y
			texldb r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex0; level = 5, w = -0";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -0.0
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex0; level = 5, w = -1";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -1.0
            dcl_2d s0
			
			mov r1, c0
			mov r1.x, t0.x
			mov r1.y, t0.y
			texldb r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, Tex0; level = 5, w = -2";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -2.0
            dcl_2d s0
			
			mov r2, c0
			mov r2.x, t0.x
			mov r2.y, t0.y
			texldb r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, Tex0; level = 5, w = -3";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -3.0
            dcl_2d s0
			
			mov r3, c0
			mov r3.x, t0.x
			mov r3.y, t0.y
			texldb r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0; w = 0, MaxMipLevel[0] = 0";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r1, t0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0; w = 0, MaxMipLevel[0] = 1";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r1, t0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0; w = 0, MaxMipLevel[0] = 2";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r1, t0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0; w = 0, MaxMipLevel[0] = 3";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r1, t0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0; w = 0, MaxMipLevel[0] = 4";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r1, t0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0; w = 0, MaxMipLevel[0] = 5";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r1, t0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0; w = 0, MaxMipLevel[0] = 6";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r1, t0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex0; w = 0, MaxMipLevel[0] = 0";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r0, t0
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex0; w = 0, MaxMipLevel[0] = 1";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r1, t0
			texldb r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, Tex0; w = 0, MaxMipLevel[0] = 2";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r2, t0
			texldb r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, Tex0; w = 0, MaxMipLevel[0] = 3";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r3, t0
			texldb r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, Tex0; w = 0, MaxMipLevel[0] = 4";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r4, t0
			texldb r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, Tex0; w = 0, MaxMipLevel[0] = 5";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r5, t0
			texldb r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, Tex0; w = 0, MaxMipLevel[0] = 6";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r6, t0
			texldb r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0; w = 0, MipMapLODBias[0] = 0";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r1, t0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0; w = 0, MipMapLODBias[0] = 1";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r1, t0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0; w = 0, MipMapLODBias[0] = 2";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r1, t0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0; w = 0, MipMapLODBias[0] = 3";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r1, t0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0; w = 0, MipMapLODBias[0] = 4";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r1, t0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0; w = 0, MipMapLODBias[0] = 5";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r1, t0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex0; w = 0, MipMapLODBias[0] = 6";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			texldb r1, t0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex0; w = 0, MipMapLODBias[0] = 0";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r0, t0
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex0; w = 0, MipMapLODBias[0] = 1";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r1, t0
			texldb r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, Tex0; w = 0, MipMapLODBias[0] = 2";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r2, t0
			texldb r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, Tex0; w = 0, MipMapLODBias[0] = 3";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r3, t0
			texldb r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, Tex0; w = 0, MipMapLODBias[0] = 4";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r4, t0
			texldb r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, Tex0; w = 0, MipMapLODBias[0] = 5";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r5, t0
			texldb r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, Tex0; w = 0, MipMapLODBias[0] = 6";
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
			ps_2_x
			dcl t0
            dcl_2d s0
			
			mov r6, t0
			texldb r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex0; w = 3, MipMapLODBias[0] = -0";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s0
			
			mov r0, t2
			mov r0.w, c0.w
			texldb r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex0; w = 3, MipMapLODBias[0] = -1";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s0
			
			mov r1, t2
			mov r1.w, c0.w
			texldb r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, Tex0; w = 3, MipMapLODBias[0] = -2";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s0
			
			mov r2, t2
			mov r2.w, c0.w
			texldb r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, Tex0; w = 3, MipMapLODBias[0] = -3";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s0
			
			mov r3, t2
			mov r3.w, c0.w
			texldb r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, Tex0; w = 3, MipMapLODBias[0] = -4";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s0
			
			mov r4, t2
			mov r4.w, c0.w
			texldb r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, Tex0; w = 3, MipMapLODBias[0] = -5";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s0
			
			mov r5, t2
			mov r5.w, c0.w
			texldb r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t1, Tex7";
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
			ps_2_x
			dcl t1
            dcl_2d s7
			
			texldb r0, t1, s7
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex7";
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
			ps_2_x
			dcl t1
            dcl_2d s7
			
			mov r1, t1
			texldb r0, r1, s7
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t1, Tex7; writemask=xyzw";
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
			ps_2_x
			dcl t1
            dcl_2d s7
			
			texldb r1.xyzw, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex7; writemask=xyzw";
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
			ps_2_x
			dcl t1
            dcl_2d s7
			
			mov r1, t1
			texldb r0.xyzw, r1, s7
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : t1, Tex7";
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
			ps_2_x
			dcl t1
            dcl_2d s7
			
			texldb_pp r0, t1, s7
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : r1, Tex7";
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
			ps_2_x
			dcl t1
            dcl_2d s7
			
			mov r1, t1
			texldb_pp r0, r1, s7
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex7; w = 0";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s7
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex7; w = 1";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 1.0
            dcl_2d s7
			
			mov r1, c0
			mov r1.x, t0.x
			mov r1.y, t0.y
			texldb r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, Tex7; w = 2";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 2.0
            dcl_2d s7
			
			mov r2, c0
			mov r2.x, t0.x
			mov r2.y, t0.y
			texldb r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, Tex7; w = 3";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 3.0
            dcl_2d s7
			
			mov r3, c0
			mov r3.x, t0.x
			mov r3.y, t0.y
			texldb r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex7; level = 5, w = -0";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_2>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -0.0
            dcl_2d s7
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex7; level = 5, w = -1";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_2>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -1.0
            dcl_2d s7
			
			mov r1, c0
			mov r1.x, t0.x
			mov r1.y, t0.y
			texldb r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, Tex7; level = 5, w = -2";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_2>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -2.0
            dcl_2d s7
			
			mov r2, c0
			mov r2.x, t0.x
			mov r2.y, t0.y
			texldb r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, Tex7; level = 5, w = -3";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_2>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -3.0
            dcl_2d s7
			
			mov r3, c0
			mov r3.x, t0.x
			mov r3.y, t0.y
			texldb r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex7; w = 0, MaxMipLevel[7] = 0";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			texldb r1, t0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex7; w = 0, MaxMipLevel[7] = 1";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			texldb r1, t0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex7; w = 0, MaxMipLevel[7] = 2";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			texldb r1, t0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex7; w = 0, MaxMipLevel[7] = 3";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			texldb r1, t0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex7; w = 0, MaxMipLevel[7] = 4";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			texldb r1, t0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex7; w = 0, MaxMipLevel[7] = 5";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			texldb r1, t0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex7; w = 0, MaxMipLevel[7] = 6";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			texldb r1, t0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex7; w = 0, MaxMipLevel[7] = 0";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			mov r0, t0
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex7; w = 0, MaxMipLevel[7] = 1";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			mov r1, t0
			texldb r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, Tex7; w = 0, MaxMipLevel[7] = 2";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			mov r2, t0
			texldb r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, Tex7; w = 0, MaxMipLevel[7] = 3";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			mov r3, t0
			texldb r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, Tex7; w = 0, MaxMipLevel[7] = 4";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			mov r4, t0
			texldb r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, Tex7; w = 0, MaxMipLevel[7] = 5";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			mov r5, t0
			texldb r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, Tex7; w = 0, MaxMipLevel[7] = 6";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			mov r6, t0
			texldb r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex7; w = 0, MipMapLODBias[7] = 0";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			texldb r1, t0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex7; w = 0, MipMapLODBias[7] = 1";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			texldb r1, t0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex7; w = 0, MipMapLODBias[7] = 2";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			texldb r1, t0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex7; w = 0, MipMapLODBias[7] = 3";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			texldb r1, t0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex7; w = 0, MipMapLODBias[7] = 4";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			texldb r1, t0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex7; w = 0, MipMapLODBias[7] = 5";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			texldb r1, t0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex7; w = 0, MipMapLODBias[7] = 6";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			texldb r1, t0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex7; w = 0, MipMapLODBias[7] = 0";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			mov r0, t0
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex7; w = 0, MipMapLODBias[7] = 1";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			mov r1, t0
			texldb r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, Tex7; w = 0, MipMapLODBias[7] = 2";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			mov r2, t0
			texldb r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, Tex7; w = 0, MipMapLODBias[7] = 3";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			mov r3, t0
			texldb r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, Tex7; w = 0, MipMapLODBias[7] = 4";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			mov r4, t0
			texldb r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, Tex7; w = 0, MipMapLODBias[7] = 5";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			mov r5, t0
			texldb r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, Tex7; w = 0, MipMapLODBias[7] = 6";
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
			ps_2_x
			dcl t0
            dcl_2d s7
			
			mov r6, t0
			texldb r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex7; w = 3, MipMapLODBias[7] = -0";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s7
			
			mov r0, t2
			mov r0.w, c0.w
			texldb r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex7; w = 3, MipMapLODBias[7] = -1";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s7
			
			mov r1, t2
			mov r1.w, c0.w
			texldb r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, Tex7; w = 3, MipMapLODBias[7] = -2";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s7
			
			mov r2, t2
			mov r2.w, c0.w
			texldb r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, Tex7; w = 3, MipMapLODBias[7] = -3";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s7
			
			mov r3, t2
			mov r3.w, c0.w
			texldb r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, Tex7; w = 3, MipMapLODBias[7] = -4";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s7
			
			mov r4, t2
			mov r4.w, c0.w
			texldb r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, Tex7; w = 3, MipMapLODBias[7] = -5";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s7
			
			mov r5, t2
			mov r5.w, c0.w
			texldb r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex12; w = 0";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_2d s12
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex12; w = 1";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 1.0
            dcl_2d s12
			
			mov r1, c0
			mov r1.x, t0.x
			mov r1.y, t0.y
			texldb r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, Tex12; w = 2";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 2.0
            dcl_2d s12
			
			mov r2, c0
			mov r2.x, t0.x
			mov r2.y, t0.y
			texldb r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, Tex12; w = 3";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 3.0
            dcl_2d s12
			
			mov r3, c0
			mov r3.x, t0.x
			mov r3.y, t0.y
			texldb r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex12; level = 5, w = -0";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_2>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -0.0
            dcl_2d s12
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex12; level = 5, w = -1";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_2>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -1.0
            dcl_2d s12
			
			mov r1, c0
			mov r1.x, t0.x
			mov r1.y, t0.y
			texldb r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, Tex12; level = 5, w = -2";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_2>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -2.0
            dcl_2d s12
			
			mov r2, c0
			mov r2.x, t0.x
			mov r2.y, t0.y
			texldb r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, Tex12; level = 5, w = -3";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_2>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -3.0
            dcl_2d s12
			
			mov r3, c0
			mov r3.x, t0.x
			mov r3.y, t0.y
			texldb r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex12; w = 0, MaxMipLevel[12] = 0";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			texldb r1, t0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex12; w = 0, MaxMipLevel[12] = 1";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			texldb r1, t0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex12; w = 0, MaxMipLevel[12] = 2";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			texldb r1, t0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex12; w = 0, MaxMipLevel[12] = 3";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			texldb r1, t0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex12; w = 0, MaxMipLevel[12] = 4";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			texldb r1, t0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex12; w = 0, MaxMipLevel[12] = 5";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			texldb r1, t0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex12; w = 0, MaxMipLevel[12] = 6";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			texldb r1, t0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex12; w = 0, MaxMipLevel[12] = 0";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			mov r0, t0
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex12; w = 0, MaxMipLevel[12] = 1";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			mov r1, t0
			texldb r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, Tex12; w = 0, MaxMipLevel[12] = 2";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			mov r2, t0
			texldb r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, Tex12; w = 0, MaxMipLevel[12] = 3";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			mov r3, t0
			texldb r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, Tex12; w = 0, MaxMipLevel[12] = 4";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			mov r4, t0
			texldb r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, Tex12; w = 0, MaxMipLevel[12] = 5";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			mov r5, t0
			texldb r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, Tex12; w = 0, MaxMipLevel[12] = 6";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			mov r6, t0
			texldb r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex12; w = 0, MipMapLODBias[12] = 0";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			texldb r1, t0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex12; w = 0, MipMapLODBias[12] = 1";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			texldb r1, t0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex12; w = 0, MipMapLODBias[12] = 2";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			texldb r1, t0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex12; w = 0, MipMapLODBias[12] = 3";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			texldb r1, t0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex12; w = 0, MipMapLODBias[12] = 4";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			texldb r1, t0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex12; w = 0, MipMapLODBias[12] = 5";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			texldb r1, t0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, Tex12; w = 0, MipMapLODBias[12] = 6";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			texldb r1, t0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex12; w = 0, MipMapLODBias[12] = 0";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			mov r0, t0
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex12; w = 0, MipMapLODBias[12] = 1";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			mov r1, t0
			texldb r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, Tex12; w = 0, MipMapLODBias[12] = 2";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			mov r2, t0
			texldb r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, Tex12; w = 0, MipMapLODBias[12] = 3";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			mov r3, t0
			texldb r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, Tex12; w = 0, MipMapLODBias[12] = 4";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			mov r4, t0
			texldb r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, Tex12; w = 0, MipMapLODBias[12] = 5";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			mov r5, t0
			texldb r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, Tex12; w = 0, MipMapLODBias[12] = 6";
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
			ps_2_x
			dcl t0
            dcl_2d s12
			
			mov r6, t0
			texldb r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, Tex12; w = 3, MipMapLODBias[12] = -0";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s12
			
			mov r0, t2
			mov r0.w, c0.w
			texldb r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, Tex12; w = 3, MipMapLODBias[12] = -1";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s12
			
			mov r1, t2
			mov r1.w, c0.w
			texldb r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, Tex12; w = 3, MipMapLODBias[12] = -2";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s12
			
			mov r2, t2
			mov r2.w, c0.w
			texldb r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, Tex12; w = 3, MipMapLODBias[12] = -3";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s12
			
			mov r3, t2
			mov r3.w, c0.w
			texldb r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, Tex12; w = 3, MipMapLODBias[12] = -4";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s12
			
			mov r4, t2
			mov r4.w, c0.w
			texldb r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, Tex12; w = 3, MipMapLODBias[12] = -5";
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
			ps_2_x
			def		c0,	0,	0,	0,	3
			dcl t2
            dcl_2d s12
			
			mov r5, t2
			mov r5.w, c0.w
			texldb r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}