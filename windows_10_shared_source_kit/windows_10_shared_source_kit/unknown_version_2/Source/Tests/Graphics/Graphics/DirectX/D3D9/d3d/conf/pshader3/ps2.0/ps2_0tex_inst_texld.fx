#include "include.fx"

Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 5";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 0, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s0
			
			texld r0.xyzw, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 1, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s0
			
			texld r0.xyzw, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 2, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s0
			
			texld r0.xyzw, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 3, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s0
			
			texld r0.xyzw, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 4, writemask=xyzw";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s0
			
			texld r0.xyzw, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 5, writemask=xyzw";
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
            dcl_2d s0
			
			texld r0.xyzw, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, MaxMipLevel[0] = 0";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, MaxMipLevel[0] = 1";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, MaxMipLevel[0] = 2";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, MaxMipLevel[0] = 3";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, MaxMipLevel[0] = 4";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, MaxMipLevel[0] = 5";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 0, MipMapLODBias[0] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 0, MipMapLODBias[0] = 1";
	String	Shape = "TexTL0";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 0, MipMapLODBias[0] = 2";
	String	Shape = "TexTL0";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 0, MipMapLODBias[0] = 3";
	String	Shape = "TexTL0";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 0, MipMapLODBias[0] = 4";
	String	Shape = "TexTL0";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 0, MipMapLODBias[0] = 5";
	String	Shape = "TexTL0";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 5, MipMapLODBias[0] = -0";
	String	Shape = "TexTL5";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 5, MipMapLODBias[0] = -1";
	String	Shape = "TexTL5";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 5, MipMapLODBias[0] = -2";
	String	Shape = "TexTL5";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 5, MipMapLODBias[0] = -3";
	String	Shape = "TexTL5";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 5, MipMapLODBias[0] = -4";
	String	Shape = "TexTL5";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex0, level = 5, MipMapLODBias[0] = -5";
	String	Shape = "TexTL5";
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
            dcl_2d s0
			
			texld r0, t0, s0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 1";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 2";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 3";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 4";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 5";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 0, writemask=xyzw";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0.xyzw, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 1, writemask=xyzw";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0.xyzw, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 2, writemask=xyzw";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0.xyzw, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 3, writemask=xyzw";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0.xyzw, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 4, writemask=xyzw";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0.xyzw, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 5, writemask=xyzw";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0.xyzw, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, MaxMipLevel[10] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, MaxMipLevel[10] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 1;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, MaxMipLevel[10] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 2;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, MaxMipLevel[10] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 3;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, MaxMipLevel[10] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, MaxMipLevel[10] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 5;
        MipMapLODBias[10] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 0, MipMapLODBias[10] = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 0, MipMapLODBias[10] = 1";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 0, MipMapLODBias[10] = 2";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 0, MipMapLODBias[10] = 3";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 0, MipMapLODBias[10] = 4";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 0, MipMapLODBias[10] = 5";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 5, MipMapLODBias[10] = -0";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 5, MipMapLODBias[10] = -1";
	String	Shape = "TexTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 5, MipMapLODBias[10] = -2";
	String	Shape = "TexTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 5, MipMapLODBias[10] = -3";
	String	Shape = "TexTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 5, MipMapLODBias[10] = -4";
	String	Shape = "TexTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex10, level = 5, MipMapLODBias[10] = -5";
	String	Shape = "TexTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s10
			
			texld r0, t0, s10
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 1";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 2";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 3";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 4";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 5";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 0, writemask=xyzw";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0.xyzw, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 1, writemask=xyzw";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0.xyzw, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 2, writemask=xyzw";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0.xyzw, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 3, writemask=xyzw";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0.xyzw, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 4, writemask=xyzw";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0.xyzw, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 5, writemask=xyzw";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0.xyzw, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, MaxMipLevel[15] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, MaxMipLevel[15] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 1;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, MaxMipLevel[15] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 2;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, MaxMipLevel[15] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, MaxMipLevel[15] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 4;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, MaxMipLevel[15] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 5;
        MipMapLODBias[15] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 0, MipMapLODBias[15] = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 0, MipMapLODBias[15] = 1";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 0, MipMapLODBias[15] = 2";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 0, MipMapLODBias[15] = 3";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 0, MipMapLODBias[15] = 4";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 0, MipMapLODBias[15] = 5";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 5, MipMapLODBias[15] = -0";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 5, MipMapLODBias[15] = -1";
	String	Shape = "TexTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 5, MipMapLODBias[15] = -2";
	String	Shape = "TexTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 5, MipMapLODBias[15] = -3";
	String	Shape = "TexTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 5, MipMapLODBias[15] = -4";
	String	Shape = "TexTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_2_0 texld : Tex15, level = 5, MipMapLODBias[15] = -5";
	String	Shape = "TexTL5";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            dcl_2d s15
			
			texld r0, t0, s15
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}