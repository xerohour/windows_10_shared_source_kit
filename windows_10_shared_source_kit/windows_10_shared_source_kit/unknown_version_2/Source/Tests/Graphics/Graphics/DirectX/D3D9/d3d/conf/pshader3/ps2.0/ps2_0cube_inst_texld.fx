#include "include.fx"

Technique 
< 
	String	Name = "texld : TexCube5, level = 0";
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
			ps_2_0
			dcl             t0.xyz
            dcl_cube        s5
			
			texld   r0, t0, s5
			mov     oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : TexCube5, level = 1";
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
			ps_2_0
			dcl             t0.xyz
            dcl_cube        s5
			
			texld   r0,     t0, s5
			mov     oC0,    r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "texld : TexCube5, level = 2";
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
			ps_2_0
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
	String	Name = "texld : TexCube5, level = 3";
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
	        ps_2_0
			dcl t0.xyz
			dcl_cube s5
				
			texld   r0, t0, s5
			mov     oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "texld : TexCube5, level = 4";
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
			ps_2_0
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
	String	Name = "texld : TexCube5, level = 5";
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
			ps_2_0
			dcl t0.xyz
			dcl_cube s5
				
			texld   r0, t0, s5
			mov     oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "texld : TexCube5, level = 0, writemask=xyzw";
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
			ps_2_0
			dcl     t0.xyz
			dcl_cube s5
				
			texld   r0.xyzw, t0, s5
			mov     oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "texld : TexCube5, level = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s5
			
			texld r0.xyzw, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube5, level = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s5
			
			texld r0.xyzw, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube5, level = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s5
			
			texld r0.xyzw, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube5, level = 4, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s5
			
			texld r0.xyzw, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube5, level = 5, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s5
			
			texld r0.xyzw, t0, s5
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube5, MaxMipLevel[5] = 0";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, MaxMipLevel[5] = 1";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, MaxMipLevel[5] = 2";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, MaxMipLevel[5] = 3";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, MaxMipLevel[5] = 4";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, MaxMipLevel[5] = 5";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, MaxMipLevel[5] = 6";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, MipMapLODBias[5] = 0";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, MipMapLODBias[5] = 1";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, MipMapLODBias[5] = 2";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, MipMapLODBias[5] = 3";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, MipMapLODBias[5] = 4";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, MipMapLODBias[5] = 5";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, MipMapLODBias[5] = 6";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, level = 5, MipMapLODBias[5] = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, level = 5, MipMapLODBias[5] = -1";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, level = 5, MipMapLODBias[5] = -2";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, level = 5, MipMapLODBias[5] = -3";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, level = 5, MipMapLODBias[5] = -4";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, level = 5, MipMapLODBias[5] = -5";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube5, level = 5, MipMapLODBias[5] = -6";
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
			ps_2_0
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
	String	Name = "ps_2_0 texld : TexCube9, level = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0.xyzw, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0.xyzw, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0.xyzw, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0.xyzw, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 4, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0.xyzw, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 5, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0.xyzw, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, MaxMipLevel[9] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, MaxMipLevel[9] = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, MaxMipLevel[9] = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, MaxMipLevel[9] = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, MaxMipLevel[9] = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, MaxMipLevel[9] = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, MaxMipLevel[9] = 6";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, MipMapLODBias[9] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, MipMapLODBias[9] = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, MipMapLODBias[9] = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, MipMapLODBias[9] = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, MipMapLODBias[9] = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, MipMapLODBias[9] = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, MipMapLODBias[9] = 6";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 5, MipMapLODBias[9] = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 5, MipMapLODBias[9] = -1";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 5, MipMapLODBias[9] = -2";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 5, MipMapLODBias[9] = -3";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 5, MipMapLODBias[9] = -4";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 5, MipMapLODBias[9] = -5";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube9, level = 5, MipMapLODBias[9] = -6";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s9
			
			texld r0, t0, s9
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 0, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0.xyzw, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 1, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0.xyzw, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 2, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0.xyzw, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 3, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0.xyzw, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 4, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0.xyzw, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 5, writemask=xyzw";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0.xyzw, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, MaxMipLevel[14] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, MaxMipLevel[14] = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, MaxMipLevel[14] = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, MaxMipLevel[14] = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, MaxMipLevel[14] = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, MaxMipLevel[14] = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, MaxMipLevel[14] = 6";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, MipMapLODBias[14] = 0";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, MipMapLODBias[14] = 1";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, MipMapLODBias[14] = 2";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, MipMapLODBias[14] = 3";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, MipMapLODBias[14] = 4";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, MipMapLODBias[14] = 5";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, MipMapLODBias[14] = 6";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 5, MipMapLODBias[14] = -0";
	String	Shape = "CubeTL5";
>
{
    Pass P0
    {        
		Texture[14] = <TexCube_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 5, MipMapLODBias[14] = -1";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 5, MipMapLODBias[14] = -2";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 5, MipMapLODBias[14] = -3";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 5, MipMapLODBias[14] = -4";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 5, MipMapLODBias[14] = -5";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : TexCube14, level = 5, MipMapLODBias[14] = -6";
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
			ps_2_0
			dcl t0.xyz
            dcl_cube s14
			
			texld r0, t0, s14
			mov oC0, r0
        };
         
		VertexShader = <VolTLVS>;
   }
}
