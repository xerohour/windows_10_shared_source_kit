#include "include.fx"

Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexCube3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_cube s3
			
			mov r0, v0
			texldb r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c0, TexCube3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.99f, 0.01f, 0.01f, 0.f
			dcl_cube s3
			
			texldb r1, c0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_cube s3
			
			texldb r1.xyzw, v0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexCube3; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_cube s3
			
			mov r0, v0
			texldb r1.xyzw, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_3_0 texldb : c0, TexCube3; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.01f, 0.99f, 0.01f, 0.f
			dcl_cube s3
			
			texldb r1.xyzw, c0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : v0, TexCube3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_cube s3
			
			texldb_pp r1, v0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : r0, TexCube3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_cube s3
			
			mov r0, v0
			texldb_pp r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_3_0 texldb_pp : c0, TexCube3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.01f, 0.01f, 0.99f, 0.f
			dcl_cube s3
			
			texldb_pp r1, c0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexCube3; w = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexCube3; w = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s3
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexCube3; w = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_cube s3
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexCube3; w = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_cube s3
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexCube3; w = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_cube s3
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexCube3; w = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_cube s3
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexCube3; w = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s3
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexCube;, level = 5, w = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_1>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_cube s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexCube;, level = 5, w = -1";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_1>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_cube s3
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexCube;, level = 5, w = -2";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_1>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_cube s3
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexCube;, level = 5, w = -3";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_1>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_cube s3
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexCube;, level = 5, w = -4";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_1>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -4.0, -4.0, -4.0, -4.0
            dcl_cube s3
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexCube;, level = 5, w = -5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_1>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -5.0, -5.0, -5.0, -5.0
            dcl_cube s3
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexCube;, level = 5, w = -6";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_1>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -6.0, -6.0, -6.0, -6.0
            dcl_cube s3
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MaxMipLevel[3] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MaxMipLevel[3] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 1;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MaxMipLevel[3] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 2;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MaxMipLevel[3] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MaxMipLevel[3] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 4;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MaxMipLevel[3] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 5;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MaxMipLevel[3] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 6;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MaxMipLevel[3] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 7;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexCube3; w = 0, MaxMipLevel[3] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r0, v0
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexCube3; w = 0, MaxMipLevel[3] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 1;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r1, v0
			texldb r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexCube3; w = 0, MaxMipLevel[3] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 2;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r2, v0
			texldb r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexCube3; w = 0, MaxMipLevel[3] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r3, v0
			texldb r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexCube3; w = 0, MaxMipLevel[3] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 4;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r4, v0
			texldb r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexCube3; w = 0, MaxMipLevel[3] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 5;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r5, v0
			texldb r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexCube3; w = 0, MaxMipLevel[3] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 6;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r6, v0
			texldb r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r7, TexCube3; w = 0, MaxMipLevel[3] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 7;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r7, v0
			texldb r1, r7, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MipMapLODBias[3] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MipMapLODBias[3] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MipMapLODBias[3] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MipMapLODBias[3] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MipMapLODBias[3] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MipMapLODBias[3] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MipMapLODBias[3] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 0, MipMapLODBias[3] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			texldb r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexCube3; w = 0, MipMapLODBias[3] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r0, v0
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexCube3; w = 0, MipMapLODBias[3] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r1, v0
			texldb r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexCube3; w = 0, MipMapLODBias[3] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r2, v0
			texldb r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexCube3; w = 0, MipMapLODBias[3] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r3, v0
			texldb r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexCube3; w = 0, MipMapLODBias[3] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r4, v0
			texldb r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexCube3; w = 0, MipMapLODBias[3] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r5, v0
			texldb r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexCube3; w = 0, MipMapLODBias[3] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r6, v0
			texldb r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r7, TexCube3; w = 0, MipMapLODBias[3] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s3
			
			mov r7, v0
			texldb r1, r7, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube3; w = 6, MipMapLODBias[3] = -0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			texldb r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube3; w = 6, MipMapLODBias[3] = -1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			texldb r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube3; w = 6, MipMapLODBias[3] = -2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			texldb r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube3; w = 6, MipMapLODBias[3] = -3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			texldb r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube3; w = 6, MipMapLODBias[3] = -4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			texldb r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube3; w = 6, MipMapLODBias[3] = -5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			texldb r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube3; w = 6, MipMapLODBias[3] = -6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			texldb r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube3; w = 6, MipMapLODBias[3] = -7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			texldb r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube3; w = 6, MipMapLODBias[3] = -0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			mov r0, v2
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v1, TexCube3; w = 6, MipMapLODBias[3] = -1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			mov r1, v2
			texldb r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube3; w = 6, MipMapLODBias[3] = -2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			mov r2, v2
			texldb r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v3, TexCube3; w = 6, MipMapLODBias[3] = -3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			mov r3, v2
			texldb r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v4, TexCube3; w = 6, MipMapLODBias[3] = -4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			mov r4, v2
			texldb r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v5, TexCube3; w = 6, MipMapLODBias[3] = -5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			mov r5, v2
			texldb r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v6, TexCube3; w = 6, MipMapLODBias[3] = -6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			mov r6, v2
			texldb r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v7, TexCube3; w = 6, MipMapLODBias[3] = -7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s3
			
			mov r7, v2
			texldb r1, r7, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v1, TexCube10";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
			dcl_cube s10
			
			texldb r1, v1, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexCube10";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
			dcl_cube s10
			
			mov r1, v1
			texldb r1, r1, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c1, TexCube10";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
			def c1, 0.99f, 0.01f, 0.01f, 1.f
			dcl_cube s10
			
			texldb r1, c1, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v1, TexCube10; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
			dcl_cube s10
			
			texldb r1.xyzw, v1, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexCube10; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
			dcl_cube s10
			
			mov r1, v1
			texldb r1.xyzw, r1, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_3_0 texldb : c1, TexCube10; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
			def c1, 0.01f, 0.99f, 0.01f, 1.f
			dcl_cube s10
			
			texldb r1.xyzw, c1, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : v1, TexCube10";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
			dcl_cube s10
			
			texldb_pp r1, v1, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : r1, TexCube10";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
			dcl_cube s10
			
			mov r1, v1
			texldb_pp r1, r1, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_3_0 texldb_pp : c1, TexCube10";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
			def c1, 0.01f, 0.01f, 0.99f, 1.f
			dcl_cube s10
			
			texldb_pp r1, c1, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexCube10; w = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexCube10; w = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexCube10; w = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexCube10; w = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexCube10; w = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexCube10; w = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexCube10; w = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexCube;, level = 5, w = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_1>;
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
			def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_cube s10
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexCube;, level = 5, w = -1";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_1>;
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
			def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_cube s10
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexCube;, level = 5, w = -2";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_1>;
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
			def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_cube s10
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexCube;, level = 5, w = -3";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_1>;
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
			def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_cube s10
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexCube;, level = 5, w = -4";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_1>;
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
			def  c0, -4.0, -4.0, -4.0, -4.0
            dcl_cube s10
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexCube;, level = 5, w = -5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_1>;
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
			def  c0, -5.0, -5.0, -5.0, -5.0
            dcl_cube s10
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexCube;, level = 5, w = -6";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_1>;
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
			def  c0, -6.0, -6.0, -6.0, -6.0
            dcl_cube s10
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s10
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MaxMipLevel[10] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MaxMipLevel[10] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MaxMipLevel[10] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MaxMipLevel[10] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MaxMipLevel[10] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MaxMipLevel[10] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MaxMipLevel[10] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MaxMipLevel[10] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 7;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexCube10; w = 0, MaxMipLevel[10] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r0, v0
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexCube10; w = 0, MaxMipLevel[10] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r1, v0
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexCube10; w = 0, MaxMipLevel[10] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r2, v0
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexCube10; w = 0, MaxMipLevel[10] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r3, v0
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexCube10; w = 0, MaxMipLevel[10] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r4, v0
			texldb r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexCube10; w = 0, MaxMipLevel[10] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r5, v0
			texldb r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexCube10; w = 0, MaxMipLevel[10] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r6, v0
			texldb r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r7, TexCube10; w = 0, MaxMipLevel[10] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 7;
        MipMapLODBias[10] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s10
			
			mov r7, v0
			texldb r1, r7, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MipMapLODBias[10] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MipMapLODBias[10] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MipMapLODBias[10] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MipMapLODBias[10] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MipMapLODBias[10] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MipMapLODBias[10] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MipMapLODBias[10] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 0, MipMapLODBias[10] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s10
			
			texldb r1, v0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexCube10; w = 0, MipMapLODBias[10] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r0, v0
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexCube10; w = 0, MipMapLODBias[10] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r1, v0
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexCube10; w = 0, MipMapLODBias[10] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r2, v0
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexCube10; w = 0, MipMapLODBias[10] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r3, v0
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexCube10; w = 0, MipMapLODBias[10] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r4, v0
			texldb r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexCube10; w = 0, MipMapLODBias[10] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r5, v0
			texldb r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexCube10; w = 0, MipMapLODBias[10] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r6, v0
			texldb r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r7, TexCube10; w = 0, MipMapLODBias[10] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s10
			
			mov r7, v0
			texldb r1, r7, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube10; w = 6, MipMapLODBias[10] = -0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube10; w = 6, MipMapLODBias[10] = -1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube10; w = 6, MipMapLODBias[10] = -2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube10; w = 6, MipMapLODBias[10] = -3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube10; w = 6, MipMapLODBias[10] = -4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube10; w = 6, MipMapLODBias[10] = -5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube10; w = 6, MipMapLODBias[10] = -6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			texldb r1, v2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube10; w = 6, MipMapLODBias[10] = -7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s10
			
			texldb r1, v2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube10; w = 6, MipMapLODBias[10] = -0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r0, v2
			texldb r1, r0, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v1, TexCube10; w = 6, MipMapLODBias[10] = -1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r1, v2
			texldb r1, r1, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube10; w = 6, MipMapLODBias[10] = -2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r2, v2
			texldb r1, r2, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v3, TexCube10; w = 6, MipMapLODBias[10] = -3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r3, v2
			texldb r1, r3, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v4, TexCube10; w = 6, MipMapLODBias[10] = -4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r4, v2
			texldb r1, r4, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v5, TexCube10; w = 6, MipMapLODBias[10] = -5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r5, v2
			texldb r1, r5, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v6, TexCube10; w = 6, MipMapLODBias[10] = -6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
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
            dcl_cube s10
			
			mov r6, v2
			texldb r1, r6, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v7, TexCube10; w = 6, MipMapLODBias[10] = -7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[10] = <TexCube_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s10
			
			mov r7, v2
			texldb r1, r7, s10
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube15";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
			dcl_cube s15
			
			texldb r1, v2, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexCube15";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
			dcl_cube s15
			
			mov r2, v2
			texldb r1, r2, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : c2, TexCube15";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
			def c2, 0.99f, 0.01f, 0.01f, 2.f
			dcl_cube s15
			
			texldb r1, c2, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube15; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
			dcl_cube s15
			
			texldb r1.xyzw, v2, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexCube15; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
			dcl_cube s15
			
			mov r2, v2
			texldb r1.xyzw, r2, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_3_0 texldb : c2, TexCube15; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
			def c2, 0.01f, 0.99f, 0.01f, 2.f
			dcl_cube s15
			
			texldb r1.xyzw, c2, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : v2, TexCube15";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
			dcl_cube s15
			
			texldb_pp r1, v2, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb_pp : r2, TexCube15";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
			dcl_cube s15
			
			mov r2, v2
			texldb_pp r1, r2, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_3_0 texldb_pp : c2, TexCube15";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
			def c2, 0.01f, 0.01f, 0.99f, 2.f
			dcl_cube s15
			
			texldb_pp r1, c2, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexCube15; w = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexCube15; w = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexCube15; w = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexCube15; w = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexCube15; w = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexCube15; w = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexCube15; w = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexCube;, level = 5, w = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_1>;
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
			def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_cube s15
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb r1, r0, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexCube;, level = 5, w = -1";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_1>;
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
			def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_cube s15
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldb r1, r1, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexCube;, level = 5, w = -2";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_1>;
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
			def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_cube s15
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldb r1, r2, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexCube;, level = 5, w = -3";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_1>;
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
			def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_cube s15
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldb r1, r3, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexCube;, level = 5, w = -4";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_1>;
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
			def  c0, -4.0, -4.0, -4.0, -4.0
            dcl_cube s15
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldb r1, r4, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexCube;, level = 5, w = -5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_1>;
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
			def  c0, -5.0, -5.0, -5.0, -5.0
            dcl_cube s15
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldb r1, r5, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexCube;, level = 5, w = -6";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_1>;
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
			def  c0, -6.0, -6.0, -6.0, -6.0
            dcl_cube s15
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldb r1, r6, s15
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MaxMipLevel[15] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MaxMipLevel[15] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MaxMipLevel[15] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MaxMipLevel[15] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MaxMipLevel[15] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MaxMipLevel[15] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MaxMipLevel[15] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MaxMipLevel[15] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 7;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexCube15; w = 0, MaxMipLevel[15] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r0, v0
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexCube15; w = 0, MaxMipLevel[15] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r1, v0
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexCube15; w = 0, MaxMipLevel[15] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r2, v0
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexCube15; w = 0, MaxMipLevel[15] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r3, v0
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexCube15; w = 0, MaxMipLevel[15] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r4, v0
			texldb r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexCube15; w = 0, MaxMipLevel[15] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r5, v0
			texldb r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexCube15; w = 0, MaxMipLevel[15] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r6, v0
			texldb r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r7, TexCube15; w = 0, MaxMipLevel[15] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 7;
        MipMapLODBias[15] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s15
			
			mov r7, v0
			texldb r1, r7, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MipMapLODBias[15] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MipMapLODBias[15] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MipMapLODBias[15] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MipMapLODBias[15] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MipMapLODBias[15] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MipMapLODBias[15] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MipMapLODBias[15] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 0, MipMapLODBias[15] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s15
			
			texldb r1, v0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r0, TexCube15; w = 0, MipMapLODBias[15] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r0, v0
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r1, TexCube15; w = 0, MipMapLODBias[15] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r1, v0
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r2, TexCube15; w = 0, MipMapLODBias[15] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r2, v0
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r3, TexCube15; w = 0, MipMapLODBias[15] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r3, v0
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r4, TexCube15; w = 0, MipMapLODBias[15] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r4, v0
			texldb r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r5, TexCube15; w = 0, MipMapLODBias[15] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r5, v0
			texldb r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r6, TexCube15; w = 0, MipMapLODBias[15] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r6, v0
			texldb r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : r7, TexCube15; w = 0, MipMapLODBias[15] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s15
			
			mov r7, v0
			texldb r1, r7, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube15; w = 6, MipMapLODBias[15] = -0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube15; w = 6, MipMapLODBias[15] = -1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube15; w = 6, MipMapLODBias[15] = -2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube15; w = 6, MipMapLODBias[15] = -3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube15; w = 6, MipMapLODBias[15] = -4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube15; w = 6, MipMapLODBias[15] = -5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube15; w = 6, MipMapLODBias[15] = -6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			texldb r1, v2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube15; w = 6, MipMapLODBias[15] = -7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s15
			
			texldb r1, v2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v0, TexCube15; w = 6, MipMapLODBias[15] = -0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r0, v2
			texldb r1, r0, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v1, TexCube15; w = 6, MipMapLODBias[15] = -1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r1, v2
			texldb r1, r1, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v2, TexCube15; w = 6, MipMapLODBias[15] = -2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r2, v2
			texldb r1, r2, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v3, TexCube15; w = 6, MipMapLODBias[15] = -3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r3, v2
			texldb r1, r3, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v4, TexCube15; w = 6, MipMapLODBias[15] = -4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r4, v2
			texldb r1, r4, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v5, TexCube15; w = 6, MipMapLODBias[15] = -5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r5, v2
			texldb r1, r5, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v6, TexCube15; w = 6, MipMapLODBias[15] = -6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
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
            dcl_cube s15
			
			mov r6, v2
			texldb r1, r6, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldb : v7, TexCube15; w = 6, MipMapLODBias[15] = -7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[15] = <TexCube_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s15
			
			mov r7, v2
			texldb r1, r7, s15
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
