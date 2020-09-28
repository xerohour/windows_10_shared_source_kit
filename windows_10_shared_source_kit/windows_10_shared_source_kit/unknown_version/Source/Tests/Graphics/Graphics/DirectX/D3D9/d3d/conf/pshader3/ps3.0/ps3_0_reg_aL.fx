#include "include.fx"

Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s0; Tex2D_0, i0 = (1, 0, 0, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s0
			loop aL, i0
				texld r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s1; Tex2D_0, i0 = (1, 0, 1, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s1
			loop aL, i0
				texld r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s2; Tex2D_0, i0 = (1, 0, 2, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s2
			loop aL, i0
				texld r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s3; Tex2D_0, i0 = (1, 1, 0, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s3
			loop aL, i0
				texld r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s4; Tex2D_0, i0 = (1, 1, 1, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s4
			loop aL, i0
				texld r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s5; Tex2D_0, i0 = (1, 1, 2, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s5
			loop aL, i0
				texld r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s6; Tex2D_0, i0 = (1, 2, 0, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s6
			loop aL, i0
				texld r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s7; Tex2D_0, i0 = (1, 2, 1, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s7
			loop aL, i0
				texld r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s8; Tex2D_0, i0 = (1, 2, 2, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s8
			loop aL, i0
				texld r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s9; Tex2D_0, i0 = (2, 0, 0, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
		MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s9
			loop aL, i0
				texld r0, v[aL], s9
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s0; Tex2D_0, i0 = (2, 0, 1, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s0
			loop aL, i0
				texld r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s1; Tex2D_0, i0 = (2, 0, 2, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s1
			loop aL, i0
				texld r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s2; Tex2D_0, i0 = (2, 1, 0, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s2
			loop aL, i0
				texld r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s3; Tex2D_0, i0 = (2, 1, 1, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s3
			loop aL, i0
				texld r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s4; Tex2D_0, i0 = (2, 2, 0, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s4
			loop aL, i0
				texld r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s5; Tex2D_0, i0 = (3, 0, 0, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s5
			loop aL, i0
				texld r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s6; Tex2D_0, i0 = (3, 0, 1, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s6
			loop aL, i0
				texld r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s7; Tex2D_0, i0 = (3, 1, 0, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s7
			loop aL, i0
				texld r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s8; Tex2D_0, i0 = (3, 2, 0, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s8
			loop aL, i0
				texld r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s0; TexCube_0, i0 = (1, 0, 0, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[0] = <TexCube_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s0
			loop aL, i0
				texld r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s1; TexCube_0, i0 = (1, 0, 1, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s1
			loop aL, i0
				texld r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s2; TexCube_0, i0 = (1, 0, 2, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s2
			loop aL, i0
				texld r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s3; TexCube_0, i0 = (1, 1, 0, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s3
			loop aL, i0
				texld r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s4; TexCube_0, i0 = (1, 1, 1, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[4] = <TexCube_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s4
			loop aL, i0
				texld r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s5; TexCube_0, i0 = (1, 1, 2, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s5
			loop aL, i0
				texld r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s6; TexCube_0, i0 = (1, 2, 0, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s6
			loop aL, i0
				texld r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s7; TexCube_0, i0 = (1, 2, 1, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s7
			loop aL, i0
				texld r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s8; TexCube_0, i0 = (1, 2, 2, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s8
			loop aL, i0
				texld r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s9; TexCube_0, i0 = (2, 0, 0, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
		MipFilter[9] = None;
		MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s9
			loop aL, i0
				texld r0, v[aL], s9
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s0; TexCube_0, i0 = (2, 0, 1, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[0] = <TexCube_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s0
			loop aL, i0
				texld r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s1; TexCube_0, i0 = (2, 0, 2, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s1
			loop aL, i0
				texld r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s2; TexCube_0, i0 = (2, 1, 0, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s2
			loop aL, i0
				texld r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s3; TexCube_0, i0 = (2, 1, 1, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s3
			loop aL, i0
				texld r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s4; TexCube_0, i0 = (2, 2, 0, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[4] = <TexCube_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s4
			loop aL, i0
				texld r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s5; TexCube_0, i0 = (3, 0, 0, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s5
			loop aL, i0
				texld r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s6; TexCube_0, i0 = (3, 0, 1, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s6
			loop aL, i0
				texld r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s7; TexCube_0, i0 = (3, 1, 0, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s7
			loop aL, i0
				texld r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s8; TexCube_0, i0 = (3, 2, 0, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s8
			loop aL, i0
				texld r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s0; TexVol_0, i0 = (1, 0, 0, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s0
			loop aL, i0
				texld r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s1; TexVol_0, i0 = (1, 0, 1, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s1
			loop aL, i0
				texld r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s2; TexVol_0, i0 = (1, 0, 2, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s2
			loop aL, i0
				texld r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s3; TexVol_0, i0 = (1, 1, 0, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s3
			loop aL, i0
				texld r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s4; TexVol_0, i0 = (1, 1, 1, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s4
			loop aL, i0
				texld r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s5; TexVol_0, i0 = (1, 1, 2, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s5
			loop aL, i0
				texld r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s6; TexVol_0, i0 = (1, 2, 0, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s6
			loop aL, i0
				texld r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s7; TexVol_0, i0 = (1, 2, 1, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s7
			loop aL, i0
				texld r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s8; TexVol_0, i0 = (1, 2, 2, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s8
			loop aL, i0
				texld r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s9; TexVol_0, i0 = (2, 0, 0, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
		MipFilter[9] = None;
		MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s9
			loop aL, i0
				texld r0, v[aL], s9
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s0; TexVol_0, i0 = (2, 0, 1, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s0
			loop aL, i0
				texld r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s1; TexVol_0, i0 = (2, 0, 2, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s1
			loop aL, i0
				texld r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s2; TexVol_0, i0 = (2, 1, 0, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s2
			loop aL, i0
				texld r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s3; TexVol_0, i0 = (2, 1, 1, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s3
			loop aL, i0
				texld r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s4; TexVol_0, i0 = (2, 2, 0, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s4
			loop aL, i0
				texld r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s5; TexVol_0, i0 = (3, 0, 0, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s5
			loop aL, i0
				texld r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s6; TexVol_0, i0 = (3, 0, 1, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s6
			loop aL, i0
				texld r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s7; TexVol_0, i0 = (3, 1, 0, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s7
			loop aL, i0
				texld r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texld r0, v[aL], s8; TexVol_0, i0 = (3, 2, 0, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s8
			loop aL, i0
				texld r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s0; Tex2D_0, i0 = (1, 0, 0, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s0
			loop aL, i0
				texldb r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s1; Tex2D_0, i0 = (1, 0, 1, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s1
			loop aL, i0
				texldb r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s2; Tex2D_0, i0 = (1, 0, 2, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s2
			loop aL, i0
				texldb r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s3; Tex2D_0, i0 = (1, 1, 0, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s3
			loop aL, i0
				texldb r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s4; Tex2D_0, i0 = (1, 1, 1, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s4
			loop aL, i0
				texldb r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s5; Tex2D_0, i0 = (1, 1, 2, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s5
			loop aL, i0
				texldb r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s6; Tex2D_0, i0 = (1, 2, 0, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s6
			loop aL, i0
				texldb r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s7; Tex2D_0, i0 = (1, 2, 1, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s7
			loop aL, i0
				texldb r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s8; Tex2D_0, i0 = (1, 2, 2, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s8
			loop aL, i0
				texldb r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s9; Tex2D_0, i0 = (2, 0, 0, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
		MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s9
			loop aL, i0
				texldb r0, v[aL], s9
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s0; Tex2D_0, i0 = (2, 0, 1, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s0
			loop aL, i0
				texldb r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s1; Tex2D_0, i0 = (2, 0, 2, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s1
			loop aL, i0
				texldb r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s2; Tex2D_0, i0 = (2, 1, 0, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s2
			loop aL, i0
				texldb r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s3; Tex2D_0, i0 = (2, 1, 1, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s3
			loop aL, i0
				texldb r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s4; Tex2D_0, i0 = (2, 2, 0, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s4
			loop aL, i0
				texldb r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s5; Tex2D_0, i0 = (3, 0, 0, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s5
			loop aL, i0
				texldb r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s6; Tex2D_0, i0 = (3, 0, 1, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s6
			loop aL, i0
				texldb r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s7; Tex2D_0, i0 = (3, 1, 0, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s7
			loop aL, i0
				texldb r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s8; Tex2D_0, i0 = (3, 2, 0, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s8
			loop aL, i0
				texldb r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s0; TexCube_0, i0 = (1, 0, 0, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[0] = <TexCube_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s0
			loop aL, i0
				texldb r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s1; TexCube_0, i0 = (1, 0, 1, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s1
			loop aL, i0
				texldb r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s2; TexCube_0, i0 = (1, 0, 2, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s2
			loop aL, i0
				texldb r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s3; TexCube_0, i0 = (1, 1, 0, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s3
			loop aL, i0
				texldb r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s4; TexCube_0, i0 = (1, 1, 1, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[4] = <TexCube_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s4
			loop aL, i0
				texldb r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s5; TexCube_0, i0 = (1, 1, 2, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s5
			loop aL, i0
				texldb r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s6; TexCube_0, i0 = (1, 2, 0, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s6
			loop aL, i0
				texldb r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s7; TexCube_0, i0 = (1, 2, 1, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s7
			loop aL, i0
				texldb r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s8; TexCube_0, i0 = (1, 2, 2, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s8
			loop aL, i0
				texldb r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s9; TexCube_0, i0 = (2, 0, 0, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
		MipFilter[9] = None;
		MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s9
			loop aL, i0
				texldb r0, v[aL], s9
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s0; TexCube_0, i0 = (2, 0, 1, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[0] = <TexCube_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s0
			loop aL, i0
				texldb r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s1; TexCube_0, i0 = (2, 0, 2, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s1
			loop aL, i0
				texldb r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s2; TexCube_0, i0 = (2, 1, 0, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s2
			loop aL, i0
				texldb r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s3; TexCube_0, i0 = (2, 1, 1, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s3
			loop aL, i0
				texldb r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s4; TexCube_0, i0 = (2, 2, 0, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[4] = <TexCube_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s4
			loop aL, i0
				texldb r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s5; TexCube_0, i0 = (3, 0, 0, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s5
			loop aL, i0
				texldb r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s6; TexCube_0, i0 = (3, 0, 1, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s6
			loop aL, i0
				texldb r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s7; TexCube_0, i0 = (3, 1, 0, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s7
			loop aL, i0
				texldb r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s8; TexCube_0, i0 = (3, 2, 0, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s8
			loop aL, i0
				texldb r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s0; TexVol_0, i0 = (1, 0, 0, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s0
			loop aL, i0
				texldb r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s1; TexVol_0, i0 = (1, 0, 1, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s1
			loop aL, i0
				texldb r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s2; TexVol_0, i0 = (1, 0, 2, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s2
			loop aL, i0
				texldb r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s3; TexVol_0, i0 = (1, 1, 0, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s3
			loop aL, i0
				texldb r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s4; TexVol_0, i0 = (1, 1, 1, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s4
			loop aL, i0
				texldb r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s5; TexVol_0, i0 = (1, 1, 2, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s5
			loop aL, i0
				texldb r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s6; TexVol_0, i0 = (1, 2, 0, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s6
			loop aL, i0
				texldb r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s7; TexVol_0, i0 = (1, 2, 1, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s7
			loop aL, i0
				texldb r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s8; TexVol_0, i0 = (1, 2, 2, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s8
			loop aL, i0
				texldb r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s9; TexVol_0, i0 = (2, 0, 0, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
		MipFilter[9] = None;
		MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s9
			loop aL, i0
				texldb r0, v[aL], s9
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s0; TexVol_0, i0 = (2, 0, 1, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s0
			loop aL, i0
				texldb r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s1; TexVol_0, i0 = (2, 0, 2, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s1
			loop aL, i0
				texldb r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s2; TexVol_0, i0 = (2, 1, 0, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s2
			loop aL, i0
				texldb r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s3; TexVol_0, i0 = (2, 1, 1, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s3
			loop aL, i0
				texldb r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s4; TexVol_0, i0 = (2, 2, 0, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s4
			loop aL, i0
				texldb r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s5; TexVol_0, i0 = (3, 0, 0, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s5
			loop aL, i0
				texldb r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s6; TexVol_0, i0 = (3, 0, 1, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s6
			loop aL, i0
				texldb r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s7; TexVol_0, i0 = (3, 1, 0, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s7
			loop aL, i0
				texldb r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldb r0, v[aL], s8; TexVol_0, i0 = (3, 2, 0, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s8
			loop aL, i0
				texldb r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s0; Tex2D_0, i0 = (1, 0, 0, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s0
			loop aL, i0
				texldl r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s1; Tex2D_0, i0 = (1, 0, 1, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s1
			loop aL, i0
				texldl r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s2; Tex2D_0, i0 = (1, 0, 2, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s2
			loop aL, i0
				texldl r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s3; Tex2D_0, i0 = (1, 1, 0, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s3
			loop aL, i0
				texldl r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s4; Tex2D_0, i0 = (1, 1, 1, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s4
			loop aL, i0
				texldl r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s5; Tex2D_0, i0 = (1, 1, 2, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s5
			loop aL, i0
				texldl r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s6; Tex2D_0, i0 = (1, 2, 0, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s6
			loop aL, i0
				texldl r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s7; Tex2D_0, i0 = (1, 2, 1, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s7
			loop aL, i0
				texldl r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s8; Tex2D_0, i0 = (1, 2, 2, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s8
			loop aL, i0
				texldl r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s9; Tex2D_0, i0 = (2, 0, 0, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
		MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s9
			loop aL, i0
				texldl r0, v[aL], s9
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s0; Tex2D_0, i0 = (2, 0, 1, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s0
			loop aL, i0
				texldl r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s1; Tex2D_0, i0 = (2, 0, 2, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s1
			loop aL, i0
				texldl r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s2; Tex2D_0, i0 = (2, 1, 0, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s2
			loop aL, i0
				texldl r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s3; Tex2D_0, i0 = (2, 1, 1, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s3
			loop aL, i0
				texldl r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s4; Tex2D_0, i0 = (2, 2, 0, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s4
			loop aL, i0
				texldl r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s5; Tex2D_0, i0 = (3, 0, 0, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s5
			loop aL, i0
				texldl r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s6; Tex2D_0, i0 = (3, 0, 1, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s6
			loop aL, i0
				texldl r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s7; Tex2D_0, i0 = (3, 1, 0, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s7
			loop aL, i0
				texldl r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s8; Tex2D_0, i0 = (3, 2, 0, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s8
			loop aL, i0
				texldl r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s0; TexCube_0, i0 = (1, 0, 0, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[0] = <TexCube_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s0
			loop aL, i0
				texldl r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s1; TexCube_0, i0 = (1, 0, 1, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s1
			loop aL, i0
				texldl r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s2; TexCube_0, i0 = (1, 0, 2, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s2
			loop aL, i0
				texldl r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s3; TexCube_0, i0 = (1, 1, 0, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s3
			loop aL, i0
				texldl r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s4; TexCube_0, i0 = (1, 1, 1, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[4] = <TexCube_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s4
			loop aL, i0
				texldl r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s5; TexCube_0, i0 = (1, 1, 2, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s5
			loop aL, i0
				texldl r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s6; TexCube_0, i0 = (1, 2, 0, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s6
			loop aL, i0
				texldl r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s7; TexCube_0, i0 = (1, 2, 1, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s7
			loop aL, i0
				texldl r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s8; TexCube_0, i0 = (1, 2, 2, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s8
			loop aL, i0
				texldl r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s9; TexCube_0, i0 = (2, 0, 0, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
		MipFilter[9] = None;
		MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s9
			loop aL, i0
				texldl r0, v[aL], s9
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s0; TexCube_0, i0 = (2, 0, 1, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[0] = <TexCube_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s0
			loop aL, i0
				texldl r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s1; TexCube_0, i0 = (2, 0, 2, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s1
			loop aL, i0
				texldl r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s2; TexCube_0, i0 = (2, 1, 0, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s2
			loop aL, i0
				texldl r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s3; TexCube_0, i0 = (2, 1, 1, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s3
			loop aL, i0
				texldl r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s4; TexCube_0, i0 = (2, 2, 0, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[4] = <TexCube_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s4
			loop aL, i0
				texldl r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s5; TexCube_0, i0 = (3, 0, 0, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s5
			loop aL, i0
				texldl r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s6; TexCube_0, i0 = (3, 0, 1, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s6
			loop aL, i0
				texldl r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s7; TexCube_0, i0 = (3, 1, 0, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s7
			loop aL, i0
				texldl r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s8; TexCube_0, i0 = (3, 2, 0, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s8
			loop aL, i0
				texldl r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s0; TexVol_0, i0 = (1, 0, 0, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s0
			loop aL, i0
				texldl r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s1; TexVol_0, i0 = (1, 0, 1, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s1
			loop aL, i0
				texldl r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s2; TexVol_0, i0 = (1, 0, 2, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s2
			loop aL, i0
				texldl r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s3; TexVol_0, i0 = (1, 1, 0, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s3
			loop aL, i0
				texldl r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s4; TexVol_0, i0 = (1, 1, 1, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s4
			loop aL, i0
				texldl r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s5; TexVol_0, i0 = (1, 1, 2, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s5
			loop aL, i0
				texldl r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s6; TexVol_0, i0 = (1, 2, 0, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s6
			loop aL, i0
				texldl r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s7; TexVol_0, i0 = (1, 2, 1, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s7
			loop aL, i0
				texldl r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s8; TexVol_0, i0 = (1, 2, 2, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s8
			loop aL, i0
				texldl r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s9; TexVol_0, i0 = (2, 0, 0, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
		MipFilter[9] = None;
		MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s9
			loop aL, i0
				texldl r0, v[aL], s9
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s0; TexVol_0, i0 = (2, 0, 1, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s0
			loop aL, i0
				texldl r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s1; TexVol_0, i0 = (2, 0, 2, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s1
			loop aL, i0
				texldl r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s2; TexVol_0, i0 = (2, 1, 0, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s2
			loop aL, i0
				texldl r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s3; TexVol_0, i0 = (2, 1, 1, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s3
			loop aL, i0
				texldl r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s4; TexVol_0, i0 = (2, 2, 0, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s4
			loop aL, i0
				texldl r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s5; TexVol_0, i0 = (3, 0, 0, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s5
			loop aL, i0
				texldl r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s6; TexVol_0, i0 = (3, 0, 1, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s6
			loop aL, i0
				texldl r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s7; TexVol_0, i0 = (3, 1, 0, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s7
			loop aL, i0
				texldl r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldl r0, v[aL], s8; TexVol_0, i0 = (3, 2, 0, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s8
			loop aL, i0
				texldl r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s0; Tex2D_0, i0 = (1, 1, 0, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s0
			loop aL, i0
				texldp r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s1; Tex2D_0, i0 = (1, 1, 1, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s1
			loop aL, i0
				texldp r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s2; Tex2D_0, i0 = (1, 1, 2, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s2
			loop aL, i0
				texldp r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s3; Tex2D_0, i0 = (1, 2, 0, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s3
			loop aL, i0
				texldp r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s4; Tex2D_0, i0 = (1, 2, 1, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s4
			loop aL, i0
				texldp r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s5; Tex2D_0, i0 = (1, 2, 2, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s5
			loop aL, i0
				texldp r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s6; Tex2D_0, i0 = (2, 0, 1, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s6
			loop aL, i0
				texldp r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s7; Tex2D_0, i0 = (2, 0, 2, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s7
			loop aL, i0
				texldp r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s8; Tex2D_0, i0 = (2, 1, 0, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s8
			loop aL, i0
				texldp r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s9; Tex2D_0, i0 = (2, 1, 1, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
		MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s9
			loop aL, i0
				texldp r0, v[aL], s9
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s0; Tex2D_0, i0 = (2, 2, 0, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s0
			loop aL, i0
				texldp r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s1; Tex2D_0, i0 = (3, 0, 1, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s1
			loop aL, i0
				texldp r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s2; Tex2D_0, i0 = (3, 1, 0, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s2
			loop aL, i0
				texldp r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s3; Tex2D_0, i0 = (3, 2, 0, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s3
			loop aL, i0
				texldp r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s0; TexVol_0, i0 = (1, 1, 0, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s0
			loop aL, i0
				texldp r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s1; TexVol_0, i0 = (1, 1, 1, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s1
			loop aL, i0
				texldp r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s2; TexVol_0, i0 = (1, 1, 2, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s2
			loop aL, i0
				texldp r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s3; TexVol_0, i0 = (1, 2, 0, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s3
			loop aL, i0
				texldp r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s4; TexVol_0, i0 = (1, 2, 1, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s4
			loop aL, i0
				texldp r0, v[aL], s4
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s5; TexVol_0, i0 = (1, 2, 2, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s5
			loop aL, i0
				texldp r0, v[aL], s5
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s6; TexVol_0, i0 = (2, 0, 1, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s6
			loop aL, i0
				texldp r0, v[aL], s6
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s7; TexVol_0, i0 = (2, 0, 2, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s7
			loop aL, i0
				texldp r0, v[aL], s7
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s8; TexVol_0, i0 = (2, 1, 0, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s8
			loop aL, i0
				texldp r0, v[aL], s8
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s9; TexVol_0, i0 = (2, 1, 1, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
		MipFilter[9] = None;
		MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s9
			loop aL, i0
				texldp r0, v[aL], s9
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s0; TexVol_0, i0 = (2, 2, 0, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s0
			loop aL, i0
				texldp r0, v[aL], s0
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s1; TexVol_0, i0 = (3, 0, 1, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s1
			loop aL, i0
				texldp r0, v[aL], s1
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s2; TexVol_0, i0 = (3, 1, 0, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s2
			loop aL, i0
				texldp r0, v[aL], s2
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldp r0, v[aL], s3; TexVol_0, i0 = (3, 2, 0, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s3
			loop aL, i0
				texldp r0, v[aL], s3
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s0, c0.x, c0.y; Tex2D_0, i0 = (1, 0, 0, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s0
			loop aL, i0
				texldd r0, v[aL], s0, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s1, c0.x, c0.y; Tex2D_0, i0 = (1, 0, 1, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s1
			loop aL, i0
				texldd r0, v[aL], s1, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s2, c0.x, c0.y; Tex2D_0, i0 = (1, 0, 2, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s2
			loop aL, i0
				texldd r0, v[aL], s2, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s3, c0.x, c0.y; Tex2D_0, i0 = (1, 1, 0, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s3
			loop aL, i0
				texldd r0, v[aL], s3, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s4, c0.x, c0.y; Tex2D_0, i0 = (1, 1, 1, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s4
			loop aL, i0
				texldd r0, v[aL], s4, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s5, c0.x, c0.y; Tex2D_0, i0 = (1, 1, 2, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s5
			loop aL, i0
				texldd r0, v[aL], s5, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s6, c0.x, c0.y; Tex2D_0, i0 = (1, 2, 0, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s6
			loop aL, i0
				texldd r0, v[aL], s6, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s7, c0.x, c0.y; Tex2D_0, i0 = (1, 2, 1, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s7
			loop aL, i0
				texldd r0, v[aL], s7, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s8, c0.x, c0.y; Tex2D_0, i0 = (1, 2, 2, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s8
			loop aL, i0
				texldd r0, v[aL], s8, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s9, c0.x, c0.y; Tex2D_0, i0 = (2, 0, 0, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
		MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s9
			loop aL, i0
				texldd r0, v[aL], s9, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s0, c0.x, c0.y; Tex2D_0, i0 = (2, 0, 1, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s0
			loop aL, i0
				texldd r0, v[aL], s0, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s1, c0.x, c0.y; Tex2D_0, i0 = (2, 0, 2, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s1
			loop aL, i0
				texldd r0, v[aL], s1, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s2, c0.x, c0.y; Tex2D_0, i0 = (2, 1, 0, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s2
			loop aL, i0
				texldd r0, v[aL], s2, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s3, c0.x, c0.y; Tex2D_0, i0 = (2, 1, 1, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s3
			loop aL, i0
				texldd r0, v[aL], s3, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s4, c0.x, c0.y; Tex2D_0, i0 = (2, 2, 0, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s4
			loop aL, i0
				texldd r0, v[aL], s4, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s5, c0.x, c0.y; Tex2D_0, i0 = (3, 0, 0, 0) -> aL = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s5
			loop aL, i0
				texldd r0, v[aL], s5, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s6, c0.x, c0.y; Tex2D_0, i0 = (3, 0, 1, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s6
			loop aL, i0
				texldd r0, v[aL], s6, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s7, c0.x, c0.y; Tex2D_0, i0 = (3, 1, 0, 0) -> aL = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s7
			loop aL, i0
				texldd r0, v[aL], s7, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s8, c0.x, c0.y; Tex2D_0, i0 = (3, 2, 0, 0) -> aL = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_2d s8
			loop aL, i0
				texldd r0, v[aL], s8, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s0, c0.x, c0.y; TexCube_0, i0 = (1, 0, 0, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[0] = <TexCube_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s0
			loop aL, i0
				texldd r0, v[aL], s0, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s1, c0.x, c0.y; TexCube_0, i0 = (1, 0, 1, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s1
			loop aL, i0
				texldd r0, v[aL], s1, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s2, c0.x, c0.y; TexCube_0, i0 = (1, 0, 2, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s2
			loop aL, i0
				texldd r0, v[aL], s2, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s3, c0.x, c0.y; TexCube_0, i0 = (1, 1, 0, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s3
			loop aL, i0
				texldd r0, v[aL], s3, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s4, c0.x, c0.y; TexCube_0, i0 = (1, 1, 1, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[4] = <TexCube_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s4
			loop aL, i0
				texldd r0, v[aL], s4, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s5, c0.x, c0.y; TexCube_0, i0 = (1, 1, 2, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s5
			loop aL, i0
				texldd r0, v[aL], s5, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s6, c0.x, c0.y; TexCube_0, i0 = (1, 2, 0, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s6
			loop aL, i0
				texldd r0, v[aL], s6, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s7, c0.x, c0.y; TexCube_0, i0 = (1, 2, 1, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s7
			loop aL, i0
				texldd r0, v[aL], s7, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s8, c0.x, c0.y; TexCube_0, i0 = (1, 2, 2, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s8
			loop aL, i0
				texldd r0, v[aL], s8, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s9, c0.x, c0.y; TexCube_0, i0 = (2, 0, 0, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[9] = <TexCube_0>;
		MipFilter[9] = None;
		MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s9
			loop aL, i0
				texldd r0, v[aL], s9, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s0, c0.x, c0.y; TexCube_0, i0 = (2, 0, 1, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[0] = <TexCube_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s0
			loop aL, i0
				texldd r0, v[aL], s0, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s1, c0.x, c0.y; TexCube_0, i0 = (2, 0, 2, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s1
			loop aL, i0
				texldd r0, v[aL], s1, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s2, c0.x, c0.y; TexCube_0, i0 = (2, 1, 0, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[2] = <TexCube_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s2
			loop aL, i0
				texldd r0, v[aL], s2, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s3, c0.x, c0.y; TexCube_0, i0 = (2, 1, 1, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[3] = <TexCube_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s3
			loop aL, i0
				texldd r0, v[aL], s3, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s4, c0.x, c0.y; TexCube_0, i0 = (2, 2, 0, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[4] = <TexCube_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s4
			loop aL, i0
				texldd r0, v[aL], s4, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s5, c0.x, c0.y; TexCube_0, i0 = (3, 0, 0, 0) -> aL = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[5] = <TexCube_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s5
			loop aL, i0
				texldd r0, v[aL], s5, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s6, c0.x, c0.y; TexCube_0, i0 = (3, 0, 1, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[6] = <TexCube_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s6
			loop aL, i0
				texldd r0, v[aL], s6, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s7, c0.x, c0.y; TexCube_0, i0 = (3, 1, 0, 0) -> aL = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[7] = <TexCube_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s7
			loop aL, i0
				texldd r0, v[aL], s7, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s8, c0.x, c0.y; TexCube_0, i0 = (3, 2, 0, 0) -> aL = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_cube s8
			loop aL, i0
				texldd r0, v[aL], s8, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s0, c0.x, c0.y; TexVol_0, i0 = (1, 0, 0, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s0
			loop aL, i0
				texldd r0, v[aL], s0, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s1, c0.x, c0.y; TexVol_0, i0 = (1, 0, 1, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s1
			loop aL, i0
				texldd r0, v[aL], s1, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s2, c0.x, c0.y; TexVol_0, i0 = (1, 0, 2, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s2
			loop aL, i0
				texldd r0, v[aL], s2, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s3, c0.x, c0.y; TexVol_0, i0 = (1, 1, 0, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s3
			loop aL, i0
				texldd r0, v[aL], s3, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s4, c0.x, c0.y; TexVol_0, i0 = (1, 1, 1, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s4
			loop aL, i0
				texldd r0, v[aL], s4, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s5, c0.x, c0.y; TexVol_0, i0 = (1, 1, 2, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 1, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s5
			loop aL, i0
				texldd r0, v[aL], s5, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s6, c0.x, c0.y; TexVol_0, i0 = (1, 2, 0, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s6
			loop aL, i0
				texldd r0, v[aL], s6, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s7, c0.x, c0.y; TexVol_0, i0 = (1, 2, 1, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s7
			loop aL, i0
				texldd r0, v[aL], s7, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s8, c0.x, c0.y; TexVol_0, i0 = (1, 2, 2, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 1, 2, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s8
			loop aL, i0
				texldd r0, v[aL], s8, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s9, c0.x, c0.y; TexVol_0, i0 = (2, 0, 0, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
		MipFilter[9] = None;
		MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s9
			loop aL, i0
				texldd r0, v[aL], s9, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s0, c0.x, c0.y; TexVol_0, i0 = (2, 0, 1, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
		MipFilter[0] = None;
		MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s0
			loop aL, i0
				texldd r0, v[aL], s0, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s1, c0.x, c0.y; TexVol_0, i0 = (2, 0, 2, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = None;
		MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 0, 2, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s1
			loop aL, i0
				texldd r0, v[aL], s1, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s2, c0.x, c0.y; TexVol_0, i0 = (2, 1, 0, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
		MipFilter[2] = None;
		MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s2
			loop aL, i0
				texldd r0, v[aL], s2, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s3, c0.x, c0.y; TexVol_0, i0 = (2, 1, 1, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
		MipFilter[3] = None;
		MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 1, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s3
			loop aL, i0
				texldd r0, v[aL], s3, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s4, c0.x, c0.y; TexVol_0, i0 = (2, 2, 0, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = None;
		MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 2, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s4
			loop aL, i0
				texldd r0, v[aL], s4, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s5, c0.x, c0.y; TexVol_0, i0 = (3, 0, 0, 0) -> aL = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = None;
		MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s5
			loop aL, i0
				texldd r0, v[aL], s5, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s6, c0.x, c0.y; TexVol_0, i0 = (3, 0, 1, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = None;
		MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 0, 1, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s6
			loop aL, i0
				texldd r0, v[aL], s6, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s7, c0.x, c0.y; TexVol_0, i0 = (3, 1, 0, 0) -> aL = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
		MipFilter[7] = None;
		MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 1, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s7
			loop aL, i0
				texldd r0, v[aL], s7, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 : texldd r0, v[aL], s8, c0.x, c0.y; TexVol_0, i0 = (3, 2, 0, 0) -> aL = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = None;
		MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0, 0, 0, 0
			defi i0, 3, 2, 0, 0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			dcl_texcoord2 v2
			dcl_volume s8
			loop aL, i0
				texldd r0, v[aL], s8, c0.x, c0.y
			endloop
			mov oC0, r0
		};
         
		VertexShader = <TexTLVS>;
   }
}
