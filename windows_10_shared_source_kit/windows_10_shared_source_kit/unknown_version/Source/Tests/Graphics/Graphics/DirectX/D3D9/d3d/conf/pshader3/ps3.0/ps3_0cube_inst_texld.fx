#include "include.fx"

Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0(.xyz), TexCube1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexCube1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r0, v0
			texld r0, r0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, TexCube1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.99f, 0.01f, 0.01f, 0.f
            dcl_cube s1
			
			texld r0, c0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0.xyzw, v0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexCube1; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r0, v0
			texld r0.xyzw, r0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, TexCube1; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.01f, 0.99f, 0.01f, 0.f
            dcl_cube s1
			
			texld r0.xyzw, c0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v0, TexCube1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld_pp r0, v0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r0, TexCube1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r0, v0
			texld_pp r0, r0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c0, TexCube1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.01f, 0.01f, 0.99f, 0.f
            dcl_cube s1
			
			texld_pp r0, c0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; level = 0";
	String	Shape = "CubeTL0";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; level = 1";
	String	Shape = "CubeTL1";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; level = 2";
	String	Shape = "CubeTL2";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; level = 3";
	String	Shape = "CubeTL3";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; level = 4";
	String	Shape = "CubeTL4";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; level = 5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexCube1; level = 0";
	String	Shape = "CubeTL0";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r0, v0
			texld r0, r0, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexCube1; level = 1";
	String	Shape = "CubeTL1";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r1, v0
			texld r0, r1, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexCube1; level = 2";
	String	Shape = "CubeTL2";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r2, v0
			texld r0, r2, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexCube1; level = 3";
	String	Shape = "CubeTL3";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r3, v0
			texld r0, r3, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexCube1; level = 4";
	String	Shape = "CubeTL4";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r4, v0
			texld r0, r4, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexCube1; level = 5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r5, v0
			texld r0, r5, s1
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; MaxMipLevel[1] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; MaxMipLevel[1] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; MaxMipLevel[1] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 2;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; MaxMipLevel[1] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 3;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; MaxMipLevel[1] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 4;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; MaxMipLevel[1] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; MaxMipLevel[1] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 6;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexCube1; MaxMipLevel[1] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r0, v0
			texld r0, r0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexCube1; MaxMipLevel[1] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r1, v0
			texld r0, r1, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexCube1; MaxMipLevel[1] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 2;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r2, v0
			texld r0, r2, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexCube1; MaxMipLevel[1] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 3;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r3, v0
			texld r0, r3, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexCube1; MaxMipLevel[1] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 4;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r4, v0
			texld r0, r4, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexCube1; MaxMipLevel[1] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r5, v0
			texld r0, r5, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexCube1; MaxMipLevel[1] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 6;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r6, v0
			texld r0, r6, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; MipMapLODBias[1] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; MipMapLODBias[1] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; MipMapLODBias[1] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; MipMapLODBias[1] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; MipMapLODBias[1] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; MipMapLODBias[1] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; MipMapLODBias[1] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexCube1; MipMapLODBias[1] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r0, v0
			texld r0, r0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexCube1; MipMapLODBias[1] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r1, v0
			texld r0, r1, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexCube1; MipMapLODBias[1] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r2, v0
			texld r0, r2, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexCube1; MipMapLODBias[1] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r3, v0
			texld r0, r3, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexCube1; MipMapLODBias[1] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r4, v0
			texld r0, r4, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexCube1; MipMapLODBias[1] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r5, v0
			texld r0, r5, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexCube1; MipMapLODBias[1] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r6, v0
			texld r0, r6, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; level = 5, MipMapLODBias[1] = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_1>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; level = 5, MipMapLODBias[1] = -1";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_1>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; level = 5, MipMapLODBias[1] = -2";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_1>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; level = 5, MipMapLODBias[1] = -3";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_1>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; level = 5, MipMapLODBias[1] = -4";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_1>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; level = 5, MipMapLODBias[1] = -5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_1>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube1; level = 5, MipMapLODBias[1] = -6";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_1>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			texld r0, v0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexCube1; level = 5, MipMapLODBias[1] = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_1>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r0, v0
			texld r0, r0, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexCube1; level = 5, MipMapLODBias[1] = -1";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_1>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r1, v0
			texld r0, r1, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexCube1; level = 5, MipMapLODBias[1] = -2";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_1>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r2, v0
			texld r0, r2, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexCube1; level = 5, MipMapLODBias[1] = -3";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_1>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r3, v0
			texld r0, r3, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexCube1; level = 5, MipMapLODBias[1] = -4";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_1>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r4, v0
			texld r0, r4, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexCube1; level = 5, MipMapLODBias[1] = -5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_1>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r5, v0
			texld r0, r5, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexCube1; level = 5, MipMapLODBias[1] = -6";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_1>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s1
			
			mov r6, v0
			texld r0, r6, s1
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1, TexCube7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_cube s7
			
			texld r0, v1, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1(.xyz), TexCube7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1.xyz
            dcl_cube s7
			
			texld r0, v1, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexCube7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_cube s7
			
			mov r1, v1
			texld r0, r1, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, TexCube7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.99f, 0.01f, 0.01f, 1.f
            dcl_cube s7
			
			texld r0, c1, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1, TexCube7; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_cube s7
			
			texld r0.xyzw, v1, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexCube7; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_cube s7
			
			mov r1, v1
			texld r0.xyzw, r1, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, TexCube7; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.01f, 0.99f, 0.01f, 1.f
            dcl_cube s7
			
			texld r0.xyzw, c1, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v1, TexCube7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_cube s7
			
			texld_pp r0, v1, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r1, TexCube7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_cube s7
			
			mov r1, v1
			texld_pp r0, r1, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c1, TexCube7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.01f, 0.01f, 0.99f, 1.f
            dcl_cube s7
			
			texld_pp r0, c1, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; level = 0";
	String	Shape = "CubeTL0";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; level = 1";
	String	Shape = "CubeTL1";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; level = 2";
	String	Shape = "CubeTL2";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; level = 3";
	String	Shape = "CubeTL3";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; level = 4";
	String	Shape = "CubeTL4";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; level = 5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexCube7; level = 0";
	String	Shape = "CubeTL0";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r0, v0
			texld r0, r0, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexCube7; level = 1";
	String	Shape = "CubeTL1";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r1, v0
			texld r0, r1, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexCube7; level = 2";
	String	Shape = "CubeTL2";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r2, v0
			texld r0, r2, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexCube7; level = 3";
	String	Shape = "CubeTL3";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r3, v0
			texld r0, r3, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexCube7; level = 4";
	String	Shape = "CubeTL4";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r4, v0
			texld r0, r4, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexCube7; level = 5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r5, v0
			texld r0, r5, s7
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; MaxMipLevel[7] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; MaxMipLevel[7] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; MaxMipLevel[7] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; MaxMipLevel[7] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; MaxMipLevel[7] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; MaxMipLevel[7] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; MaxMipLevel[7] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexCube7; MaxMipLevel[7] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r0, v0
			texld r0, r0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexCube7; MaxMipLevel[7] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r1, v0
			texld r0, r1, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexCube7; MaxMipLevel[7] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r2, v0
			texld r0, r2, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexCube7; MaxMipLevel[7] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r3, v0
			texld r0, r3, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexCube7; MaxMipLevel[7] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r4, v0
			texld r0, r4, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexCube7; MaxMipLevel[7] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r5, v0
			texld r0, r5, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexCube7; MaxMipLevel[7] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r6, v0
			texld r0, r6, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; MipMapLODBias[7] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; MipMapLODBias[7] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; MipMapLODBias[7] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; MipMapLODBias[7] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; MipMapLODBias[7] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; MipMapLODBias[7] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; MipMapLODBias[7] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexCube7; MipMapLODBias[7] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r0, v0
			texld r0, r0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexCube7; MipMapLODBias[7] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r1, v0
			texld r0, r1, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexCube7; MipMapLODBias[7] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r2, v0
			texld r0, r2, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexCube7; MipMapLODBias[7] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r3, v0
			texld r0, r3, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexCube7; MipMapLODBias[7] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r4, v0
			texld r0, r4, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexCube7; MipMapLODBias[7] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r5, v0
			texld r0, r5, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexCube7; MipMapLODBias[7] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r6, v0
			texld r0, r6, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; level = 5, MipMapLODBias[7] = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_1>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; level = 5, MipMapLODBias[7] = -1";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_1>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; level = 5, MipMapLODBias[7] = -2";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_1>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; level = 5, MipMapLODBias[7] = -3";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_1>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; level = 5, MipMapLODBias[7] = -4";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_1>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; level = 5, MipMapLODBias[7] = -5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_1>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube7; level = 5, MipMapLODBias[7] = -6";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_1>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			texld r0, v0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexCube7; level = 5, MipMapLODBias[7] = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_1>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r0, v0
			texld r0, r0, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexCube7; level = 5, MipMapLODBias[7] = -1";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_1>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r1, v0
			texld r0, r1, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexCube7; level = 5, MipMapLODBias[7] = -2";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_1>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r2, v0
			texld r0, r2, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexCube7; level = 5, MipMapLODBias[7] = -3";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_1>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r3, v0
			texld r0, r3, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexCube7; level = 5, MipMapLODBias[7] = -4";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_1>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r4, v0
			texld r0, r4, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexCube7; level = 5, MipMapLODBias[7] = -5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_1>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r5, v0
			texld r0, r5, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexCube7; level = 5, MipMapLODBias[7] = -6";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_1>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s7
			
			mov r6, v0
			texld r0, r6, s7
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexCube12";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s12
			
			texld r0, v2, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2(.xyz), TexCube12";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2.xyz
            dcl_cube s12
			
			texld r0, v2, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexCube12";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s12
			
			mov r2, v2
			texld r0, r2, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, TexCube12";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.99f, 0.01f, 0.01f, 2.f
            dcl_cube s12
			
			texld r0, c2, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexCube12; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s12
			
			texld r0.xyzw, v2, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexCube12; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s12
			
			mov r2, v2
			texld r0.xyzw, r2, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, TexCube12; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.01f, 0.99f, 0.01f, 2.f
            dcl_cube s12
			
			texld r0.xyzw, c2, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v2, TexCube12";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s12
			
			texld_pp r0, v2, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r2, TexCube12";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_cube s12
			
			mov r2, v2
			texld_pp r0, r2, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c2, TexCube12";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.01f, 0.01f, 0.99f, 2.f
            dcl_cube s12
			
			texld_pp r0, c2, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; level = 0";
	String	Shape = "CubeTL0";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; level = 1";
	String	Shape = "CubeTL1";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; level = 2";
	String	Shape = "CubeTL2";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; level = 3";
	String	Shape = "CubeTL3";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; level = 4";
	String	Shape = "CubeTL4";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; level = 5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexCube12; level = 0";
	String	Shape = "CubeTL0";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r0, v0
			texld r0, r0, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexCube12; level = 1";
	String	Shape = "CubeTL1";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r1, v0
			texld r0, r1, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexCube12; level = 2";
	String	Shape = "CubeTL2";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r2, v0
			texld r0, r2, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexCube12; level = 3";
	String	Shape = "CubeTL3";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r3, v0
			texld r0, r3, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexCube12; level = 4";
	String	Shape = "CubeTL4";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r4, v0
			texld r0, r4, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexCube12; level = 5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r5, v0
			texld r0, r5, s12
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; MaxMipLevel[12] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; MaxMipLevel[12] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; MaxMipLevel[12] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; MaxMipLevel[12] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; MaxMipLevel[12] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; MaxMipLevel[12] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; MaxMipLevel[12] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexCube12; MaxMipLevel[12] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r0, v0
			texld r0, r0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexCube12; MaxMipLevel[12] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r1, v0
			texld r0, r1, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexCube12; MaxMipLevel[12] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r2, v0
			texld r0, r2, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexCube12; MaxMipLevel[12] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r3, v0
			texld r0, r3, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexCube12; MaxMipLevel[12] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r4, v0
			texld r0, r4, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexCube12; MaxMipLevel[12] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r5, v0
			texld r0, r5, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexCube12; MaxMipLevel[12] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r6, v0
			texld r0, r6, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; MipMapLODBias[12] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; MipMapLODBias[12] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; MipMapLODBias[12] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; MipMapLODBias[12] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; MipMapLODBias[12] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; MipMapLODBias[12] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; MipMapLODBias[12] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexCube12; MipMapLODBias[12] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r0, v0
			texld r0, r0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexCube12; MipMapLODBias[12] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r1, v0
			texld r0, r1, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexCube12; MipMapLODBias[12] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r2, v0
			texld r0, r2, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexCube12; MipMapLODBias[12] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r3, v0
			texld r0, r3, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexCube12; MipMapLODBias[12] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r4, v0
			texld r0, r4, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexCube12; MipMapLODBias[12] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r5, v0
			texld r0, r5, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexCube12; MipMapLODBias[12] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r6, v0
			texld r0, r6, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; level = 5, MipMapLODBias[12] = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_1>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; level = 5, MipMapLODBias[12] = -1";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_1>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; level = 5, MipMapLODBias[12] = -2";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_1>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; level = 5, MipMapLODBias[12] = -3";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_1>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; level = 5, MipMapLODBias[12] = -4";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_1>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; level = 5, MipMapLODBias[12] = -5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_1>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexCube12; level = 5, MipMapLODBias[12] = -6";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_1>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			texld r0, v0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexCube12; level = 5, MipMapLODBias[12] = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_1>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r0, v0
			texld r0, r0, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexCube12; level = 5, MipMapLODBias[12] = -1";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_1>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r1, v0
			texld r0, r1, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexCube12; level = 5, MipMapLODBias[12] = -2";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_1>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r2, v0
			texld r0, r2, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexCube12; level = 5, MipMapLODBias[12] = -3";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_1>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r3, v0
			texld r0, r3, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexCube12; level = 5, MipMapLODBias[12] = -4";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_1>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r4, v0
			texld r0, r4, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexCube12; level = 5, MipMapLODBias[12] = -5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_1>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r5, v0
			texld r0, r5, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexCube12; level = 5, MipMapLODBias[12] = -6";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[12] = <TexCube_1>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_cube s12
			
			mov r6, v0
			texld r0, r6, s12
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
