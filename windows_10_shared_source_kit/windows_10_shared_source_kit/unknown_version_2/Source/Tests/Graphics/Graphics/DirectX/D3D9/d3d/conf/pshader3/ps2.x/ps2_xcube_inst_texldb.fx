#include "include.fx"

Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3";
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
			ps_2_x
			dcl t0
			dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexCube3";
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
			ps_2_x
			dcl t0
			dcl_cube s3
			
			mov r0, t0
			texldb r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; writemask=xyzw";
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
			ps_2_x
			dcl t0
			dcl_cube s3
			
			texldb r1.xyzw, t0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexCube3; writemask=xyzw";
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
			ps_2_x
			dcl t0
			dcl_cube s3
			
			mov r0, t0
			texldb r1.xyzw, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : t0, TexCube3";
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
			ps_2_x
			dcl t0
			dcl_cube s3
			
			texldb_pp r1, t0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : r0, TexCube3";
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
			ps_2_x
			dcl t0
			dcl_cube s3
			
			mov r0, t0
			texldb_pp r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexCube3; w = 0";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexCube3; w = 1";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 1.0
            dcl_cube s3
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexCube3; w = 2";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 2.0
            dcl_cube s3
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexCube3; w = 3";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 3.0
            dcl_cube s3
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexCube;, level = 5, w = -0";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -0.0
            dcl_cube s3
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexCube;, level = 5, w = -1";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -1.0
            dcl_cube s3
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexCube;, level = 5, w = -2";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -2.0
            dcl_cube s3
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexCube;, level = 5, w = -3";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -3.0
            dcl_cube s3
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MaxMipLevel[3] = 0";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MaxMipLevel[3] = 1";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MaxMipLevel[3] = 2";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MaxMipLevel[3] = 3";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MaxMipLevel[3] = 4";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MaxMipLevel[3] = 5";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MaxMipLevel[3] = 6";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MaxMipLevel[3] = 7";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexCube3; w = 0, MaxMipLevel[3] = 0";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r0, t0
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexCube3; w = 0, MaxMipLevel[3] = 1";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r1, t0
			texldb r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexCube3; w = 0, MaxMipLevel[3] = 2";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r2, t0
			texldb r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexCube3; w = 0, MaxMipLevel[3] = 3";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r3, t0
			texldb r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexCube3; w = 0, MaxMipLevel[3] = 4";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r4, t0
			texldb r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexCube3; w = 0, MaxMipLevel[3] = 5";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r5, t0
			texldb r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexCube3; w = 0, MaxMipLevel[3] = 6";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r6, t0
			texldb r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r7, TexCube3; w = 0, MaxMipLevel[3] = 7";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r7, t0
			texldb r1, r7, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MipMapLODBias[3] = 0";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MipMapLODBias[3] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MipMapLODBias[3] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MipMapLODBias[3] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MipMapLODBias[3] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MipMapLODBias[3] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MipMapLODBias[3] = 6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 0, MipMapLODBias[3] = 7";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			texldb r1, t0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexCube3; w = 0, MipMapLODBias[3] = 0";
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r0, t0
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexCube3; w = 0, MipMapLODBias[3] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r1, t0
			texldb r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexCube3; w = 0, MipMapLODBias[3] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r2, t0
			texldb r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexCube3; w = 0, MipMapLODBias[3] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r3, t0
			texldb r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexCube3; w = 0, MipMapLODBias[3] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r4, t0
			texldb r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexCube3; w = 0, MipMapLODBias[3] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r5, t0
			texldb r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexCube3; w = 0, MipMapLODBias[3] = 6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r6, t0
			texldb r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r7, TexCube3; w = 0, MipMapLODBias[3] = 7";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			dcl t0
            dcl_cube s3
			
			mov r7, t0
			texldb r1, r7, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube3; w = 3, MipMapLODBias[3] = -0";
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
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s3
			
			mov r0, t2
			mov r0.w, c0.w
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t1, TexCube3; w = 3, MipMapLODBias[3] = -1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s3
			
			mov r1, t2
			mov r1.w, c0.w
			texldb r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t2, TexCube3; w = 3, MipMapLODBias[3] = -2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s3
			
			mov r2, t2
			mov r2.w, c0.w
			texldb r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t3, TexCube3; w = 3, MipMapLODBias[3] = -3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s3
			
			mov r3, t2
			mov r3.w, c0.w
			texldb r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t4, TexCube3; w = 3, MipMapLODBias[3] = -4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s3
			
			mov r4, t2
			mov r4.w, c0.w
			texldb r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t5, TexCube3; w = 3, MipMapLODBias[3] = -5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s3
			
			mov r5, t2
			mov r5.w, c0.w
			texldb r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t1, TexCube6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
			dcl_cube s6
			
			texldb r1, t1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexCube6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
			dcl_cube s6
			
			mov r1, t1
			texldb r1, r1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t1, TexCube6; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
			dcl_cube s6
			
			texldb r1.xyzw, t1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexCube6; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
			dcl_cube s6
			
			mov r1, t1
			texldb r1.xyzw, r1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : t1, TexCube6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
			dcl_cube s6
			
			texldb_pp r1, t1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : r1, TexCube6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
			dcl_cube s6
			
			mov r1, t1
			texldb_pp r1, r1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexCube6; w = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexCube6; w = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 1.0
            dcl_cube s6
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexCube6; w = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 2.0
            dcl_cube s6
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexCube6; w = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 3.0
            dcl_cube s6
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexCube;, level = 5, w = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -0.0
            dcl_cube s6
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexCube;, level = 5, w = -1";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -1.0
            dcl_cube s6
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexCube;, level = 5, w = -2";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -2.0
            dcl_cube s6
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexCube;, level = 5, w = -3";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_1>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -3.0
            dcl_cube s6
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MaxMipLevel[6] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MaxMipLevel[6] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 1;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MaxMipLevel[6] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MaxMipLevel[6] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 3;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MaxMipLevel[6] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MaxMipLevel[6] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 5;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MaxMipLevel[6] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 6;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MaxMipLevel[6] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 7;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexCube6; w = 0, MaxMipLevel[6] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r0, t0
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexCube6; w = 0, MaxMipLevel[6] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 1;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r1, t0
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexCube6; w = 0, MaxMipLevel[6] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r2, t0
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexCube6; w = 0, MaxMipLevel[6] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 3;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r3, t0
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexCube6; w = 0, MaxMipLevel[6] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r4, t0
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexCube6; w = 0, MaxMipLevel[6] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 5;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r5, t0
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexCube6; w = 0, MaxMipLevel[6] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 6;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r6, t0
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r7, TexCube6; w = 0, MaxMipLevel[6] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 7;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r7, t0
			texldb r1, r7, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MipMapLODBias[6] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MipMapLODBias[6] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MipMapLODBias[6] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MipMapLODBias[6] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MipMapLODBias[6] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MipMapLODBias[6] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MipMapLODBias[6] = 6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 0, MipMapLODBias[6] = 7";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			texldb r1, t0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexCube6; w = 0, MipMapLODBias[6] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r0, t0
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexCube6; w = 0, MipMapLODBias[6] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r1, t0
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexCube6; w = 0, MipMapLODBias[6] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r2, t0
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexCube6; w = 0, MipMapLODBias[6] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r3, t0
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexCube6; w = 0, MipMapLODBias[6] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r4, t0
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexCube6; w = 0, MipMapLODBias[6] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r5, t0
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexCube6; w = 0, MipMapLODBias[6] = 6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r6, t0
			texldb r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r7, TexCube6; w = 0, MipMapLODBias[6] = 7";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s6
			
			mov r7, t0
			texldb r1, r7, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube6; w = 3, MipMapLODBias[6] = -0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s6
			
			mov r0, t2
			mov r0.w, c0.w
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t1, TexCube6; w = 3, MipMapLODBias[6] = -1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s6
			
			mov r1, t2
			mov r1.w, c0.w
			texldb r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t2, TexCube6; w = 3, MipMapLODBias[6] = -2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s6
			
			mov r2, t2
			mov r2.w, c0.w
			texldb r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t3, TexCube6; w = 3, MipMapLODBias[6] = -3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s6
			
			mov r3, t2
			mov r3.w, c0.w
			texldb r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t4, TexCube6; w = 3, MipMapLODBias[6] = -4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s6
			
			mov r4, t2
			mov r4.w, c0.w
			texldb r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t5, TexCube6; w = 3, MipMapLODBias[6] = -5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s6
			
			mov r5, t2
			mov r5.w, c0.w
			texldb r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t2, TexCube11";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
			dcl_cube s11
			
			texldb r1, t2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexCube11";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
			dcl_cube s11
			
			mov r2, t2
			texldb r1, r2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t2, TexCube11; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
			dcl_cube s11
			
			texldb r1.xyzw, t2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexCube11; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
			dcl_cube s11
			
			mov r2, t2
			texldb r1.xyzw, r2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : t2, TexCube11";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
			dcl_cube s11
			
			texldb_pp r1, t2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb_pp : r2, TexCube11";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
			dcl_cube s11
			
			mov r2, t2
			texldb_pp r1, r2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexCube11; w = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexCube11; w = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s11
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexCube11; w = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_cube s11
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexCube11; w = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_cube s11
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexCube;, level = 5, w = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_cube s11
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldb r1, r0, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexCube;, level = 5, w = -1";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_cube s11
			
			mov r1.x, t0.x
			mov r1.y, t0.y
			mov r1.z, t0.z
			mov r1.w, c0.w
			texldb r1, r1, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexCube;, level = 5, w = -2";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_cube s11
			
			mov r2.x, t0.x
			mov r2.y, t0.y
			mov r2.z, t0.z
			mov r2.w, c0.w
			texldb r1, r2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexCube;, level = 5, w = -3";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_1>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_cube s11
			
			mov r3.x, t0.x
			mov r3.y, t0.y
			mov r3.z, t0.z
			mov r3.w, c0.w
			texldb r1, r3, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MaxMipLevel[11] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MaxMipLevel[11] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MaxMipLevel[11] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 2;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MaxMipLevel[11] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MaxMipLevel[11] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 4;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MaxMipLevel[11] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MaxMipLevel[11] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 6;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MaxMipLevel[11] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 7;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexCube11; w = 0, MaxMipLevel[11] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r0, t0
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexCube11; w = 0, MaxMipLevel[11] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r1, t0
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexCube11; w = 0, MaxMipLevel[11] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 2;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r2, t0
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexCube11; w = 0, MaxMipLevel[11] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r3, t0
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexCube11; w = 0, MaxMipLevel[11] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 4;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r4, t0
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexCube11; w = 0, MaxMipLevel[11] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r5, t0
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexCube11; w = 0, MaxMipLevel[11] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 6;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r6, t0
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r7, TexCube11; w = 0, MaxMipLevel[11] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 7;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r7, t0
			texldb r1, r7, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MipMapLODBias[11] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MipMapLODBias[11] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MipMapLODBias[11] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MipMapLODBias[11] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MipMapLODBias[11] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MipMapLODBias[11] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MipMapLODBias[11] = 6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 0, MipMapLODBias[11] = 7";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			texldb r1, t0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r0, TexCube11; w = 0, MipMapLODBias[11] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r0, t0
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r1, TexCube11; w = 0, MipMapLODBias[11] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r1, t0
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r2, TexCube11; w = 0, MipMapLODBias[11] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r2, t0
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r3, TexCube11; w = 0, MipMapLODBias[11] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r3, t0
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r4, TexCube11; w = 0, MipMapLODBias[11] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r4, t0
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r5, TexCube11; w = 0, MipMapLODBias[11] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r5, t0
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r6, TexCube11; w = 0, MipMapLODBias[11] = 6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r6, t0
			texldb r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : r7, TexCube11; w = 0, MipMapLODBias[11] = 7";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s11
			
			mov r7, t0
			texldb r1, r7, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t0, TexCube11; w = 3, MipMapLODBias[11] = -0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s11
			
			mov r0, t2
			mov r0.w, c0.w
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t1, TexCube11; w = 3, MipMapLODBias[11] = -1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s11
			
			mov r1, t2
			mov r1.w, c0.w
			texldb r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t2, TexCube11; w = 3, MipMapLODBias[11] = -2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s11
			
			mov r2, t2
			mov r2.w, c0.w
			texldb r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t3, TexCube11; w = 3, MipMapLODBias[11] = -3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s11
			
			mov r3, t2
			mov r3.w, c0.w
			texldb r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t4, TexCube11; w = 3, MipMapLODBias[11] = -4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s11
			
			mov r4, t2
			mov r4.w, c0.w
			texldb r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldb : t5, TexCube11; w = 3, MipMapLODBias[11] = -5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def	c0,	0,	0,	0,	3
			dcl t2
            dcl_cube s11
			
			mov r5, t2
			mov r5.w, c0.w
			texldb r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
