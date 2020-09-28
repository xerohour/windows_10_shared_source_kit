#include "include.fx"

Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, TexCube2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_cube s2
			
			mov r0, v0
			texldl r1, r0, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : c0, TexCube2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.99f, 0.01f, 0.01f, 0.f
			dcl_cube s2
			
			texldl r1, c0, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_cube s2
			
			texldl r1.xyzw, v0, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, TexCube2; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_cube s2
			
			mov r0, v0
			texldl r1.xyzw, r0, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_3_0 texldl : c0, TexCube2; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.01f, 0.99f, 0.01f, 0.f
			dcl_cube s2
			
			texldl r1.xyzw, c0, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl_pp : v0, TexCube2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_cube s2
			
			texldl_pp r1, v0, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl_pp : r0, TexCube2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_cube s2
			
			mov r0, v0
			texldl_pp r1, r0, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_3_0 texldl_pp : c0, TexCube2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.01f, 0.01f, 0.99f, 0.f
			dcl_cube s2
			
			texldl_pp r1, c0, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, TexCube2; w = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl r1, r0, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, TexCube2; w = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s2
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldl r1, r1, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, TexCube2; w = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_cube s2
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldl r1, r2, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, TexCube2; w = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_cube s2
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldl r1, r3, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, TexCube2; w = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_cube s2
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldl r1, r4, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, TexCube2; w = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_cube s2
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldl r1, r5, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, TexCube2; w = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s2
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldl r1, r6, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r7, TexCube2; w = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 7.0, 7.0, 7.0, 7.0
            dcl_cube s2
			
			mov r7, v0.xyz
			mov r7.w, c0.x
			texldl r1, r7, s2
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MaxMipLevel[2] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MaxMipLevel[2] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 1;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MaxMipLevel[2] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MaxMipLevel[2] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 3;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MaxMipLevel[2] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 4;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MaxMipLevel[2] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 5;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MaxMipLevel[2] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 6;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MaxMipLevel[2] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 7;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, TexCube2; w = 0, MaxMipLevel[2] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r0, v0
			texldl r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, TexCube2; w = 0, MaxMipLevel[2] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 1;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r1, v0
			texldl r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, TexCube2; w = 0, MaxMipLevel[2] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r2, v0
			texldl r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, TexCube2; w = 0, MaxMipLevel[2] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 3;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r3, v0
			texldl r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, TexCube2; w = 0, MaxMipLevel[2] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 4;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r4, v0
			texldl r1, r4, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, TexCube2; w = 0, MaxMipLevel[2] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 5;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r5, v0
			texldl r1, r5, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, TexCube2; w = 0, MaxMipLevel[2] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 6;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r6, v0
			texldl r1, r6, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r7, TexCube2; w = 0, MaxMipLevel[2] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 7;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r7, v0
			texldl r1, r7, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MipMapLODBias[2] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MipMapLODBias[2] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MipMapLODBias[2] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MipMapLODBias[2] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MipMapLODBias[2] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MipMapLODBias[2] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MipMapLODBias[2] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube2; w = 0, MipMapLODBias[2] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			texldl r1, v0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, TexCube2; w = 0, MipMapLODBias[2] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r0, v0
			texldl r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, TexCube2; w = 0, MipMapLODBias[2] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r1, v0
			texldl r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, TexCube2; w = 0, MipMapLODBias[2] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r2, v0
			texldl r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, TexCube2; w = 0, MipMapLODBias[2] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r3, v0
			texldl r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, TexCube2; w = 0, MipMapLODBias[2] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r4, v0
			texldl r1, r4, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, TexCube2; w = 0, MipMapLODBias[2] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r5, v0
			texldl r1, r5, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, TexCube2; w = 0, MipMapLODBias[2] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r6, v0
			texldl r1, r6, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r7, TexCube2; w = 0, MipMapLODBias[2] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s2
			
			mov r7, v0
			texldl r1, r7, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube2; w = 6, MipMapLODBias[2] = -0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			texldl r1, v2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube2; w = 6, MipMapLODBias[2] = -1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			texldl r1, v2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube2; w = 6, MipMapLODBias[2] = -2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			texldl r1, v2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube2; w = 6, MipMapLODBias[2] = -3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			texldl r1, v2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube2; w = 6, MipMapLODBias[2] = -4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			texldl r1, v2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube2; w = 6, MipMapLODBias[2] = -5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			texldl r1, v2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube2; w = 6, MipMapLODBias[2] = -6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			texldl r1, v2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube2; w = 6, MipMapLODBias[2] = -7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			texldl r1, v2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, TexCube2; w = 6, MipMapLODBias[2] = -0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			mov r0, v2
			texldl r1, r0, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, TexCube2; w = 6, MipMapLODBias[2] = -1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			mov r1, v2
			texldl r1, r1, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, TexCube2; w = 6, MipMapLODBias[2] = -2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			mov r2, v2
			texldl r1, r2, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, TexCube2; w = 6, MipMapLODBias[2] = -3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			mov r3, v2
			texldl r1, r3, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, TexCube2; w = 6, MipMapLODBias[2] = -4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			mov r4, v2
			texldl r1, r4, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, TexCube2; w = 6, MipMapLODBias[2] = -5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			mov r5, v2
			texldl r1, r5, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, TexCube2; w = 6, MipMapLODBias[2] = -6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			mov r6, v2
			texldl r1, r6, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r7, TexCube2; w = 6, MipMapLODBias[2] = -7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s2
			
			mov r7, v2
			texldl r1, r7, s2
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v1, TexCube6";
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
			ps_3_0
			dcl_texcoord1 v1
			dcl_cube s6
			
			texldl r1, v1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, TexCube6";
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
			ps_3_0
			dcl_texcoord1 v1
			dcl_cube s6
			
			mov r1, v1
			texldl r1, r1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : c1, TexCube6";
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
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.99f, 0.01f, 0.01f, 1.f
			dcl_cube s6
			
			texldl r1, c1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v1, TexCube6; writemask=xyzw";
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
			ps_3_0
			dcl_texcoord1 v1
			dcl_cube s6
			
			texldl r1.xyzw, v1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, TexCube6; writemask=xyzw";
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
			ps_3_0
			dcl_texcoord1 v1
			dcl_cube s6
			
			mov r1, v1
			texldl r1.xyzw, r1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_3_0 texldl : c1, TexCube6; writemask=xyzw";
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
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.01f, 0.99f, 0.01f, 1.f
			dcl_cube s6
			
			texldl r1.xyzw, c1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl_pp : v1, TexCube6";
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
			ps_3_0
			dcl_texcoord1 v1
			dcl_cube s6
			
			texldl_pp r1, v1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl_pp : r1, TexCube6";
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
			ps_3_0
			dcl_texcoord1 v1
			dcl_cube s6
			
			mov r1, v1
			texldl_pp r1, r1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_3_0 texldl_pp : c1, TexCube6";
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
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.01f, 0.01f, 0.99f, 1.f
			dcl_cube s6
			
			texldl_pp r1, c1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, TexCube6; w = 0";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl r1, r0, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, TexCube6; w = 1";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s6
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldl r1, r1, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, TexCube6; w = 2";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_cube s6
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldl r1, r2, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, TexCube6; w = 3";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_cube s6
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldl r1, r3, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, TexCube6; w = 4";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_cube s6
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldl r1, r4, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, TexCube6; w = 5";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_cube s6
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldl r1, r5, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, TexCube6; w = 6";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s6
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldl r1, r6, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r7, TexCube6; w = 7";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 7.0, 7.0, 7.0, 7.0
            dcl_cube s6
			
			mov r7, v0.xyz
			mov r7.w, c0.x
			texldl r1, r7, s6
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MaxMipLevel[6] = 0";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MaxMipLevel[6] = 1";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MaxMipLevel[6] = 2";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MaxMipLevel[6] = 3";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MaxMipLevel[6] = 4";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MaxMipLevel[6] = 5";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MaxMipLevel[6] = 6";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MaxMipLevel[6] = 7";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, TexCube6; w = 0, MaxMipLevel[6] = 0";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r0, v0
			texldl r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, TexCube6; w = 0, MaxMipLevel[6] = 1";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r1, v0
			texldl r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, TexCube6; w = 0, MaxMipLevel[6] = 2";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r2, v0
			texldl r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, TexCube6; w = 0, MaxMipLevel[6] = 3";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r3, v0
			texldl r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, TexCube6; w = 0, MaxMipLevel[6] = 4";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r4, v0
			texldl r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, TexCube6; w = 0, MaxMipLevel[6] = 5";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r5, v0
			texldl r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, TexCube6; w = 0, MaxMipLevel[6] = 6";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r6, v0
			texldl r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r7, TexCube6; w = 0, MaxMipLevel[6] = 7";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r7, v0
			texldl r1, r7, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MipMapLODBias[6] = 0";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MipMapLODBias[6] = 1";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MipMapLODBias[6] = 2";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MipMapLODBias[6] = 3";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MipMapLODBias[6] = 4";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MipMapLODBias[6] = 5";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MipMapLODBias[6] = 6";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube6; w = 0, MipMapLODBias[6] = 7";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			texldl r1, v0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, TexCube6; w = 0, MipMapLODBias[6] = 0";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r0, v0
			texldl r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, TexCube6; w = 0, MipMapLODBias[6] = 1";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r1, v0
			texldl r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, TexCube6; w = 0, MipMapLODBias[6] = 2";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r2, v0
			texldl r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, TexCube6; w = 0, MipMapLODBias[6] = 3";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r3, v0
			texldl r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, TexCube6; w = 0, MipMapLODBias[6] = 4";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r4, v0
			texldl r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, TexCube6; w = 0, MipMapLODBias[6] = 5";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r5, v0
			texldl r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, TexCube6; w = 0, MipMapLODBias[6] = 6";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r6, v0
			texldl r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r7, TexCube6; w = 0, MipMapLODBias[6] = 7";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s6
			
			mov r7, v0
			texldl r1, r7, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube6; w = 6, MipMapLODBias[6] = -0";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			texldl r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube6; w = 6, MipMapLODBias[6] = -1";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			texldl r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube6; w = 6, MipMapLODBias[6] = -2";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			texldl r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube6; w = 6, MipMapLODBias[6] = -3";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			texldl r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube6; w = 6, MipMapLODBias[6] = -4";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			texldl r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube6; w = 6, MipMapLODBias[6] = -5";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			texldl r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube6; w = 6, MipMapLODBias[6] = -6";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			texldl r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube6; w = 6, MipMapLODBias[6] = -7";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			texldl r1, v2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, TexCube6; w = 6, MipMapLODBias[6] = -0";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			mov r0, v2
			texldl r1, r0, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, TexCube6; w = 6, MipMapLODBias[6] = -1";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			mov r1, v2
			texldl r1, r1, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, TexCube6; w = 6, MipMapLODBias[6] = -2";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			mov r2, v2
			texldl r1, r2, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, TexCube6; w = 6, MipMapLODBias[6] = -3";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			mov r3, v2
			texldl r1, r3, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, TexCube6; w = 6, MipMapLODBias[6] = -4";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			mov r4, v2
			texldl r1, r4, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, TexCube6; w = 6, MipMapLODBias[6] = -5";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			mov r5, v2
			texldl r1, r5, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, TexCube6; w = 6, MipMapLODBias[6] = -6";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			mov r6, v2
			texldl r1, r6, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r7, TexCube6; w = 6, MipMapLODBias[6] = -7";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s6
			
			mov r7, v2
			texldl r1, r7, s6
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube11";
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
			ps_3_0
			dcl_texcoord2 v2
			dcl_cube s11
			
			texldl r1, v2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, TexCube11";
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
			ps_3_0
			dcl_texcoord2 v2
			dcl_cube s11
			
			mov r2, v2
			texldl r1, r2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : c2, TexCube11";
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
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.99f, 0.01f, 0.01f, 2.f
			dcl_cube s11
			
			texldl r1, c2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube11; writemask=xyzw";
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
			ps_3_0
			dcl_texcoord2 v2
			dcl_cube s11
			
			texldl r1.xyzw, v2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, TexCube11; writemask=xyzw";
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
			ps_3_0
			dcl_texcoord2 v2
			dcl_cube s11
			
			mov r2, v2
			texldl r1.xyzw, r2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_3_0 texldl : c2, TexCube11; writemask=xyzw";
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
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.01f, 0.99f, 0.01f, 2.f
			dcl_cube s11
			
			texldl r1.xyzw, c2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl_pp : v2, TexCube11";
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
			ps_3_0
			dcl_texcoord2 v2
			dcl_cube s11
			
			texldl_pp r1, v2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl_pp : r2, TexCube11";
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
			ps_3_0
			dcl_texcoord2 v2
			dcl_cube s11
			
			mov r2, v2
			texldl_pp r1, r2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_3_0 texldl_pp : c2, TexCube11";
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
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.01f, 0.01f, 0.99f, 2.f
			dcl_cube s11
			
			texldl_pp r1, c2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, TexCube11; w = 0";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_cube s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl r1, r0, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, TexCube11; w = 1";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s11
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldl r1, r1, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, TexCube11; w = 2";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_cube s11
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldl r1, r2, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, TexCube11; w = 3";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_cube s11
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldl r1, r3, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, TexCube11; w = 4";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_cube s11
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldl r1, r4, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, TexCube11; w = 5";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_cube s11
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldl r1, r5, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, TexCube11; w = 6";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_cube s11
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldl r1, r6, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r7, TexCube11; w = 7";
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
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 7.0, 7.0, 7.0, 7.0
            dcl_cube s11
			
			mov r7, v0.xyz
			mov r7.w, c0.x
			texldl r1, r7, s11
			mov oC0, r1
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MaxMipLevel[11] = 0";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MaxMipLevel[11] = 1";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MaxMipLevel[11] = 2";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MaxMipLevel[11] = 3";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MaxMipLevel[11] = 4";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MaxMipLevel[11] = 5";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MaxMipLevel[11] = 6";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MaxMipLevel[11] = 7";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, TexCube11; w = 0, MaxMipLevel[11] = 0";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r0, v0
			texldl r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, TexCube11; w = 0, MaxMipLevel[11] = 1";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r1, v0
			texldl r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, TexCube11; w = 0, MaxMipLevel[11] = 2";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r2, v0
			texldl r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, TexCube11; w = 0, MaxMipLevel[11] = 3";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r3, v0
			texldl r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, TexCube11; w = 0, MaxMipLevel[11] = 4";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r4, v0
			texldl r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, TexCube11; w = 0, MaxMipLevel[11] = 5";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r5, v0
			texldl r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, TexCube11; w = 0, MaxMipLevel[11] = 6";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r6, v0
			texldl r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r7, TexCube11; w = 0, MaxMipLevel[11] = 7";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r7, v0
			texldl r1, r7, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MipMapLODBias[11] = 0";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MipMapLODBias[11] = 1";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MipMapLODBias[11] = 2";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MipMapLODBias[11] = 3";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MipMapLODBias[11] = 4";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MipMapLODBias[11] = 5";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MipMapLODBias[11] = 6";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v0, TexCube11; w = 0, MipMapLODBias[11] = 7";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			texldl r1, v0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, TexCube11; w = 0, MipMapLODBias[11] = 0";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r0, v0
			texldl r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, TexCube11; w = 0, MipMapLODBias[11] = 1";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r1, v0
			texldl r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, TexCube11; w = 0, MipMapLODBias[11] = 2";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r2, v0
			texldl r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, TexCube11; w = 0, MipMapLODBias[11] = 3";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r3, v0
			texldl r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, TexCube11; w = 0, MipMapLODBias[11] = 4";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r4, v0
			texldl r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, TexCube11; w = 0, MipMapLODBias[11] = 5";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r5, v0
			texldl r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, TexCube11; w = 0, MipMapLODBias[11] = 6";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r6, v0
			texldl r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r7, TexCube11; w = 0, MipMapLODBias[11] = 7";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s11
			
			mov r7, v0
			texldl r1, r7, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube11; w = 6, MipMapLODBias[11] = -0";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			texldl r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube11; w = 6, MipMapLODBias[11] = -1";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			texldl r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube11; w = 6, MipMapLODBias[11] = -2";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			texldl r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube11; w = 6, MipMapLODBias[11] = -3";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			texldl r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube11; w = 6, MipMapLODBias[11] = -4";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			texldl r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube11; w = 6, MipMapLODBias[11] = -5";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			texldl r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube11; w = 6, MipMapLODBias[11] = -6";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			texldl r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : v2, TexCube11; w = 6, MipMapLODBias[11] = -7";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			texldl r1, v2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r0, TexCube11; w = 6, MipMapLODBias[11] = -0";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			mov r0, v2
			texldl r1, r0, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r1, TexCube11; w = 6, MipMapLODBias[11] = -1";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			mov r1, v2
			texldl r1, r1, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r2, TexCube11; w = 6, MipMapLODBias[11] = -2";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			mov r2, v2
			texldl r1, r2, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r3, TexCube11; w = 6, MipMapLODBias[11] = -3";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			mov r3, v2
			texldl r1, r3, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r4, TexCube11; w = 6, MipMapLODBias[11] = -4";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			mov r4, v2
			texldl r1, r4, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r5, TexCube11; w = 6, MipMapLODBias[11] = -5";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			mov r5, v2
			texldl r1, r5, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r6, TexCube11; w = 6, MipMapLODBias[11] = -6";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			mov r6, v2
			texldl r1, r6, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldl : r7, TexCube11; w = 6, MipMapLODBias[11] = -7";
	String	Shape = "CubeTL";
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
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s11
			
			mov r7, v2
			texldl r1, r7, s11
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
