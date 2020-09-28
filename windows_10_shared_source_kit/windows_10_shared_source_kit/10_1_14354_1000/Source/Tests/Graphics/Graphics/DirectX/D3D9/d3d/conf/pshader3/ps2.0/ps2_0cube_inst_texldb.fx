#include "include.fx"

Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0";
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
			ps_2_0
			dcl t0.xyz
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 1";
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
			ps_2_0
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 2";
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
			ps_2_0
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 3";
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
			ps_2_0
			dcl t0.xyz
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, level = 5, w = -0";
	String	Shape = "CubeTL5";
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
			ps_2_0
			dcl t0.xyz
			def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, level = 5, w = -1";
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
			ps_2_0
			dcl t0.xyz
			def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, level = 5, w = -2";
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
			ps_2_0
			dcl t0.xyz
			def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, level = 5, w = -3";
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
			ps_2_0
			dcl t0.xyz
			def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, level = 5, w = -0, writemask=xyzw";
	String	Shape = "CubeTL5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, level = 5, w = -1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, level = 5, w = -2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, level = 5, w = -3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MaxMipLevel[3] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MaxMipLevel[3] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MaxMipLevel[3] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MaxMipLevel[3] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MaxMipLevel[3] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MaxMipLevel[3] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MaxMipLevel[3] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MaxMipLevel[3] = 7";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MipMapLODBias[3] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MipMapLODBias[3] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MipMapLODBias[3] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MipMapLODBias[3] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MipMapLODBias[3] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MipMapLODBias[3] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MipMapLODBias[3] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 0, MipMapLODBias[3] = 7";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 6, MipMapLODBias[3] = -0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 6, MipMapLODBias[3] = -1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 6, MipMapLODBias[3] = -2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 6, MipMapLODBias[3] = -3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 6, MipMapLODBias[3] = -4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 6, MipMapLODBias[3] = -5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 6, MipMapLODBias[3] = -6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube3, w = 6, MipMapLODBias[3] = -7";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s3
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0";
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
			ps_2_0
			dcl t0.xyz
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 1";
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
			ps_2_0
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 2";
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
			ps_2_0
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 3";
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
			ps_2_0
			dcl t0.xyz
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, level = 5, w = -0";
	String	Shape = "CubeTL5";
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
			ps_2_0
			dcl t0.xyz
			def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, level = 5, w = -1";
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
			ps_2_0
			dcl t0.xyz
			def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, level = 5, w = -2";
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
			ps_2_0
			dcl t0.xyz
			def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, level = 5, w = -3";
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
			ps_2_0
			dcl t0.xyz
			def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, level = 5, w = -0, writemask=xyzw";
	String	Shape = "CubeTL5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, level = 5, w = -1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, level = 5, w = -2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, level = 5, w = -3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MaxMipLevel[6] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MaxMipLevel[6] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MaxMipLevel[6] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MaxMipLevel[6] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MaxMipLevel[6] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MaxMipLevel[6] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MaxMipLevel[6] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MaxMipLevel[6] = 7";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MipMapLODBias[6] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MipMapLODBias[6] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MipMapLODBias[6] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MipMapLODBias[6] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MipMapLODBias[6] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MipMapLODBias[6] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MipMapLODBias[6] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 0, MipMapLODBias[6] = 7";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 6, MipMapLODBias[6] = -0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 6, MipMapLODBias[6] = -1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 6, MipMapLODBias[6] = -2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 6, MipMapLODBias[6] = -3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 6, MipMapLODBias[6] = -4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 6, MipMapLODBias[6] = -5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 6, MipMapLODBias[6] = -6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube6, w = 6, MipMapLODBias[6] = -7";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s6
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0";
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
			ps_2_0
			dcl t0.xyz
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 1";
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
			ps_2_0
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 2";
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
			ps_2_0
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 3";
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
			ps_2_0
			dcl t0.xyz
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, level = 5, w = -0";
	String	Shape = "CubeTL5";
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
			ps_2_0
			dcl t0.xyz
			def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, level = 5, w = -1";
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
			ps_2_0
			dcl t0.xyz
			def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, level = 5, w = -2";
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
			ps_2_0
			dcl t0.xyz
			def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, level = 5, w = -3";
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
			ps_2_0
			dcl t0.xyz
			def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, level = 5, w = -0, writemask=xyzw";
	String	Shape = "CubeTL5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -0.0, -0.0, -0.0, -0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, level = 5, w = -1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -1.0, -1.0, -1.0, -1.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, level = 5, w = -2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -2.0, -2.0, -2.0, -2.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, level = 5, w = -3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            def  c0, -3.0, -3.0, -3.0, -3.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1.xyzw, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MaxMipLevel[11] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MaxMipLevel[11] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MaxMipLevel[11] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MaxMipLevel[11] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MaxMipLevel[11] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MaxMipLevel[11] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MaxMipLevel[11] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MaxMipLevel[11] = 7";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MipMapLODBias[11] = 0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MipMapLODBias[11] = 1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MipMapLODBias[11] = 2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MipMapLODBias[11] = 3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MipMapLODBias[11] = 4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MipMapLODBias[11] = 5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MipMapLODBias[11] = 6";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 0, MipMapLODBias[11] = 7";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 6, MipMapLODBias[11] = -0";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 6, MipMapLODBias[11] = -1";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 6, MipMapLODBias[11] = -2";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 6, MipMapLODBias[11] = -3";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 6, MipMapLODBias[11] = -4";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 6, MipMapLODBias[11] = -5";
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
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 6, MipMapLODBias[11] = -6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldb : TexCube11, w = 6, MipMapLODBias[11] = -7";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexCube_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s11
			
			mov r0.x, t0.x  mov r0.y, t0.y mov r0.z, t0.z
			mov r0.w, c0.x
			texldb r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
