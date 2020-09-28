#include "include.fx"

Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0(.xyz), TexCube5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r0, TexCube5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r0, t0
			texld r0, r0, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0.xyzw, t0, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r0, TexCube5; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r0, t0
			texld r0.xyzw, r0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld_pp : t0, TexCube5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld_pp r0, t0, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld_pp : r0, TexCube5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r0, t0
			texld_pp r0, r0, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; level = 0";
	String	Shape = "CubeTL0";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; level = 1";
	String	Shape = "CubeTL1";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; level = 2";
	String	Shape = "CubeTL2";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; level = 3";
	String	Shape = "CubeTL3";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; level = 4";
	String	Shape = "CubeTL4";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; level = 5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r0, TexCube5; level = 0";
	String	Shape = "CubeTL0";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r0, t0
			texld r0, r0, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r1, TexCube5; level = 1";
	String	Shape = "CubeTL1";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r1, t0
			texld r0, r1, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r2, TexCube5; level = 2";
	String	Shape = "CubeTL2";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r2, t0
			texld r0, r2, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r3, TexCube5; level = 3";
	String	Shape = "CubeTL3";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r3, t0
			texld r0, r3, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r4, TexCube5; level = 4";
	String	Shape = "CubeTL4";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r4, t0
			texld r0, r4, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r5, TexCube5; level = 5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r5, t0
			texld r0, r5, s5
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; MaxMipLevel[5] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; MaxMipLevel[5] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; MaxMipLevel[5] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 2;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; MaxMipLevel[5] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; MaxMipLevel[5] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 4;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; MaxMipLevel[5] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; MaxMipLevel[5] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 6;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r0, TexCube5; MaxMipLevel[5] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r0, t0
			texld r0, r0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r1, TexCube5; MaxMipLevel[5] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r1, t0
			texld r0, r1, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r2, TexCube5; MaxMipLevel[5] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 2;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r2, t0
			texld r0, r2, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r3, TexCube5; MaxMipLevel[5] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r3, t0
			texld r0, r3, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r4, TexCube5; MaxMipLevel[5] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 4;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r4, t0
			texld r0, r4, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r5, TexCube5; MaxMipLevel[5] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r5, t0
			texld r0, r5, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r6, TexCube5; MaxMipLevel[5] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 6;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r6, t0
			texld r0, r6, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; MipMapLODBias[5] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; MipMapLODBias[5] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; MipMapLODBias[5] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; MipMapLODBias[5] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; MipMapLODBias[5] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; MipMapLODBias[5] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; MipMapLODBias[5] = 6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r0, TexCube5; MipMapLODBias[5] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r0, t0
			texld r0, r0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r1, TexCube5; MipMapLODBias[5] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r1, t0
			texld r0, r1, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r2, TexCube5; MipMapLODBias[5] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r2, t0
			texld r0, r2, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r3, TexCube5; MipMapLODBias[5] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r3, t0
			texld r0, r3, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r4, TexCube5; MipMapLODBias[5] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r4, t0
			texld r0, r4, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r5, TexCube5; MipMapLODBias[5] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r5, t0
			texld r0, r5, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r6, TexCube5; MipMapLODBias[5] = 6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r6, t0
			texld r0, r6, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; level = 5, MipMapLODBias[5] = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_1>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; level = 5, MipMapLODBias[5] = -1";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_1>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; level = 5, MipMapLODBias[5] = -2";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_1>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; level = 5, MipMapLODBias[5] = -3";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_1>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; level = 5, MipMapLODBias[5] = -4";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_1>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; level = 5, MipMapLODBias[5] = -5";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_1>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube5; level = 5, MipMapLODBias[5] = -6";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_1>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			texld r0, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r0, TexCube5; level = 5, MipMapLODBias[5] = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_1>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r0, t0
			texld r0, r0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r1, TexCube5; level = 5, MipMapLODBias[5] = -1";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_1>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r1, t0
			texld r0, r1, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r2, TexCube5; level = 5, MipMapLODBias[5] = -2";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_1>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r2, t0
			texld r0, r2, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r3, TexCube5; level = 5, MipMapLODBias[5] = -3";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_1>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r3, t0
			texld r0, r3, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r4, TexCube5; level = 5, MipMapLODBias[5] = -4";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_1>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r4, t0
			texld r0, r4, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r5, TexCube5; level = 5, MipMapLODBias[5] = -5";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_1>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r5, t0
			texld r0, r5, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r6, TexCube5; level = 5, MipMapLODBias[5] = -6";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_1>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s5
			
			mov r6, t0
			texld r0, r6, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t1, TexCube9";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_cube s9
			
			texld r0, t1, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t1(.xyz), TexCube9";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1.xyz
            dcl_cube s9
			
			texld r0, t1, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r1, TexCube9";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_cube s9
			
			mov r1, t1
			texld r0, r1, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t1, TexCube9; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_cube s9
			
			texld r0.xyzw, t1, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r1, TexCube9; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_cube s9
			
			mov r1, t1
			texld r0.xyzw, r1, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld_pp : t1, TexCube9";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_cube s9
			
			texld_pp r0, t1, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld_pp : r1, TexCube9";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_cube s9
			
			mov r1, t1
			texld_pp r0, r1, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; level = 0";
	String	Shape = "CubeTL0";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; level = 1";
	String	Shape = "CubeTL1";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; level = 2";
	String	Shape = "CubeTL2";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; level = 3";
	String	Shape = "CubeTL3";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; level = 4";
	String	Shape = "CubeTL4";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; level = 5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r0, TexCube9; level = 0";
	String	Shape = "CubeTL0";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r0, t0
			texld r0, r0, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r1, TexCube9; level = 1";
	String	Shape = "CubeTL1";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r1, t0
			texld r0, r1, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r2, TexCube9; level = 2";
	String	Shape = "CubeTL2";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r2, t0
			texld r0, r2, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r3, TexCube9; level = 3";
	String	Shape = "CubeTL3";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r3, t0
			texld r0, r3, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r4, TexCube9; level = 4";
	String	Shape = "CubeTL4";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r4, t0
			texld r0, r4, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r5, TexCube9; level = 5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r5, t0
			texld r0, r5, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; MaxMipLevel[9] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; MaxMipLevel[9] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; MaxMipLevel[9] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 2;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; MaxMipLevel[9] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; MaxMipLevel[9] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 4;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; MaxMipLevel[9] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; MaxMipLevel[9] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 6;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r0, TexCube9; MaxMipLevel[9] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r0, t0
			texld r0, r0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r1, TexCube9; MaxMipLevel[9] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r1, t0
			texld r0, r1, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r2, TexCube9; MaxMipLevel[9] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 2;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r2, t0
			texld r0, r2, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r3, TexCube9; MaxMipLevel[9] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r3, t0
			texld r0, r3, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r4, TexCube9; MaxMipLevel[9] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 4;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r4, t0
			texld r0, r4, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r5, TexCube9; MaxMipLevel[9] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r5, t0
			texld r0, r5, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r6, TexCube9; MaxMipLevel[9] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 6;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r6, t0
			texld r0, r6, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; MipMapLODBias[9] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; MipMapLODBias[9] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; MipMapLODBias[9] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; MipMapLODBias[9] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; MipMapLODBias[9] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; MipMapLODBias[9] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; MipMapLODBias[9] = 6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r0, TexCube9; MipMapLODBias[9] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r0, t0
			texld r0, r0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r1, TexCube9; MipMapLODBias[9] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r1, t0
			texld r0, r1, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r2, TexCube9; MipMapLODBias[9] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r2, t0
			texld r0, r2, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r3, TexCube9; MipMapLODBias[9] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r3, t0
			texld r0, r3, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r4, TexCube9; MipMapLODBias[9] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r4, t0
			texld r0, r4, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r5, TexCube9; MipMapLODBias[9] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r5, t0
			texld r0, r5, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r6, TexCube9; MipMapLODBias[9] = 6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r6, t0
			texld r0, r6, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; level = 5, MipMapLODBias[9] = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_1>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; level = 5, MipMapLODBias[9] = -1";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_1>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; level = 5, MipMapLODBias[9] = -2";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_1>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; level = 5, MipMapLODBias[9] = -3";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_1>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; level = 5, MipMapLODBias[9] = -4";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_1>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; level = 5, MipMapLODBias[9] = -5";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_1>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube9; level = 5, MipMapLODBias[9] = -6";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_1>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r0, TexCube9; level = 5, MipMapLODBias[9] = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_1>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r0, t0
			texld r0, r0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r1, TexCube9; level = 5, MipMapLODBias[9] = -1";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_1>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r1, t0
			texld r0, r1, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r2, TexCube9; level = 5, MipMapLODBias[9] = -2";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_1>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r2, t0
			texld r0, r2, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r3, TexCube9; level = 5, MipMapLODBias[9] = -3";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_1>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r3, t0
			texld r0, r3, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r4, TexCube9; level = 5, MipMapLODBias[9] = -4";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_1>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r4, t0
			texld r0, r4, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r5, TexCube9; level = 5, MipMapLODBias[9] = -5";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_1>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r5, t0
			texld r0, r5, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r6, TexCube9; level = 5, MipMapLODBias[9] = -6";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_1>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s9
			
			mov r6, t0
			texld r0, r6, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t2, TexCube14";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_cube s14
			
			texld r0, t2, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t2(.xyz), TexCube14";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2.xyz
            dcl_cube s14
			
			texld r0, t2, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r2, TexCube14";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_cube s14
			
			mov r2, t2
			texld r0, r2, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t2, TexCube14; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_cube s14
			
			texld r0.xyzw, t2, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r2, TexCube14; writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_cube s14
			
			mov r2, t2
			texld r0.xyzw, r2, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld_pp : t2, TexCube14";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_cube s14
			
			texld_pp r0, t2, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld_pp : r2, TexCube14";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_cube s14
			
			mov r2, t2
			texld_pp r0, r2, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; level = 0";
	String	Shape = "CubeTL0";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; level = 1";
	String	Shape = "CubeTL1";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; level = 2";
	String	Shape = "CubeTL2";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; level = 3";
	String	Shape = "CubeTL3";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; level = 4";
	String	Shape = "CubeTL4";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; level = 5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r0, TexCube14; level = 0";
	String	Shape = "CubeTL0";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r0, t0
			texld r0, r0, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r1, TexCube14; level = 1";
	String	Shape = "CubeTL1";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r1, t0
			texld r0, r1, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r2, TexCube14; level = 2";
	String	Shape = "CubeTL2";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r2, t0
			texld r0, r2, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r3, TexCube14; level = 3";
	String	Shape = "CubeTL3";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r3, t0
			texld r0, r3, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r4, TexCube14; level = 4";
	String	Shape = "CubeTL4";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r4, t0
			texld r0, r4, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r5, TexCube14; level = 5";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r5, t0
			texld r0, r5, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; MaxMipLevel[14] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; MaxMipLevel[14] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 1;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; MaxMipLevel[14] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; MaxMipLevel[14] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 3;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; MaxMipLevel[14] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 4;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; MaxMipLevel[14] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 5;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; MaxMipLevel[14] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 6;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r0, TexCube14; MaxMipLevel[14] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r0, t0
			texld r0, r0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r1, TexCube14; MaxMipLevel[14] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 1;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r1, t0
			texld r0, r1, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r2, TexCube14; MaxMipLevel[14] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r2, t0
			texld r0, r2, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r3, TexCube14; MaxMipLevel[14] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 3;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r3, t0
			texld r0, r3, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r4, TexCube14; MaxMipLevel[14] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 4;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r4, t0
			texld r0, r4, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r5, TexCube14; MaxMipLevel[14] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 5;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r5, t0
			texld r0, r5, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r6, TexCube14; MaxMipLevel[14] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 6;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r6, t0
			texld r0, r6, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; MipMapLODBias[14] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; MipMapLODBias[14] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; MipMapLODBias[14] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; MipMapLODBias[14] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; MipMapLODBias[14] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; MipMapLODBias[14] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; MipMapLODBias[14] = 6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r0, TexCube14; MipMapLODBias[14] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r0, t0
			texld r0, r0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r1, TexCube14; MipMapLODBias[14] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r1, t0
			texld r0, r1, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r2, TexCube14; MipMapLODBias[14] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r2, t0
			texld r0, r2, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r3, TexCube14; MipMapLODBias[14] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r3, t0
			texld r0, r3, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r4, TexCube14; MipMapLODBias[14] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r4, t0
			texld r0, r4, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r5, TexCube14; MipMapLODBias[14] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r5, t0
			texld r0, r5, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r6, TexCube14; MipMapLODBias[14] = 6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r6, t0
			texld r0, r6, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; level = 5, MipMapLODBias[14] = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_1>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; level = 5, MipMapLODBias[14] = -1";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_1>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; level = 5, MipMapLODBias[14] = -2";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_1>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; level = 5, MipMapLODBias[14] = -3";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_1>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; level = 5, MipMapLODBias[14] = -4";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_1>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; level = 5, MipMapLODBias[14] = -5";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_1>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : t0, TexCube14; level = 5, MipMapLODBias[14] = -6";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_1>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r0, TexCube14; level = 5, MipMapLODBias[14] = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_1>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r0, t0
			texld r0, r0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r1, TexCube14; level = 5, MipMapLODBias[14] = -1";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_1>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r1, t0
			texld r0, r1, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r2, TexCube14; level = 5, MipMapLODBias[14] = -2";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_1>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r2, t0
			texld r0, r2, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r3, TexCube14; level = 5, MipMapLODBias[14] = -3";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_1>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r3, t0
			texld r0, r3, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r4, TexCube14; level = 5, MipMapLODBias[14] = -4";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_1>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r4, t0
			texld r0, r4, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r5, TexCube14; level = 5, MipMapLODBias[14] = -5";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_1>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r5, t0
			texld r0, r5, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texld : r6, TexCube14; level = 5, MipMapLODBias[14] = -6";
	String	Shape = "CubeTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_1>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            dcl_cube s14
			
			mov r6, t0
			texld r0, r6, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
