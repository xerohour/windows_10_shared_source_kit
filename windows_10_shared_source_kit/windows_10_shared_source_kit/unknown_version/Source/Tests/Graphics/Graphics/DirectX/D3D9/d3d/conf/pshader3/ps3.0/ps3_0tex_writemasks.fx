#include "include.fx"

Technique 
< 
	String	Name = "texld : Tex0, level=0, writemask=x";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r1, c0.y
			texld r1.x, r0, s0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex1, level=1, writemask=y";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r1, c0.y
			texld r1.y, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex2, level=2, writemask=z";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r1, c0.y
			texld r1.z, r0, s2
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex3, level=3, writemask=w";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r1, c0.y
			texld r1.w, r0, s3
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex4, level=4, writemask=xy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			mov r1, c0.y
			texld r1.xy, r0, s4
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex5, level=5, writemask=xz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r1, c0.y
			texld r1.xz, r0, s5
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex6, level=0, writemask=xw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r1, c0.y
			texld r1.xw, r0, s6
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex7, level=1, writemask=yz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r1, c0.y
			texld r1.yz, r0, s7
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex8, level=2, writemask=yw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			mov r1, c0.y
			texld r1.yw, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex9, level=3, writemask=zw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r1, c0.y
			texld r1.zw, r0, s9
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex10, level=4, writemask=xyz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			mov r1, c0.y
			texld r1.xyz, r0, s10
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex11, level=5, writemask=xyw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r1, c0.y
			texld r1.xyw, r0, s11
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex12, level=0, writemask=xzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r1, c0.y
			texld r1.xzw, r0, s12
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex13, level=1, writemask=yzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r1, c0.y
			texld r1.yzw, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex14, level=2, writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			mov r1, c0.y
			texld r1.xyzw, r0, s14
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex15, level=3, writemask=x";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldl r1.x, r0, s15
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex0, level=4, writemask=y";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldl r1.y, r0, s0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex1, level=5, writemask=z";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldl r1.z, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex2, level=0, writemask=w";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldl r1.w, r0, s2
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex3, level=1, writemask=xy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldl r1.xy, r0, s3
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex4, level=2, writemask=xz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldl r1.xz, r0, s4
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex5, level=3, writemask=xw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldl r1.xw, r0, s5
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex6, level=4, writemask=yz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldl r1.yz, r0, s6
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex7, level=5, writemask=yw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldl r1.yw, r0, s7
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex8, level=0, writemask=zw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldl r1.zw, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex9, level=1, writemask=xyz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldl r1.xyz, r0, s9
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex10, level=2, writemask=xyw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldl r1.xyw, r0, s10
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex11, level=3, writemask=xzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldl r1.xzw, r0, s11
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex12, level=4, writemask=yzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldl r1.yzw, r0, s12
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex13, level=5, writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldl r1.xyzw, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex14, level=0, writemask=x";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldb r1.x, r0, s14
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex15, level=1, writemask=y";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldb r1.y, r0, s15
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex0, level=2, writemask=z";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldb r1.z, r0, s0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex1, level=3, writemask=w";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldb r1.w, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex2, level=4, writemask=xy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldb r1.xy, r0, s2
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex3, level=5, writemask=xz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldb r1.xz, r0, s3
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex4, level=0, writemask=xw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldb r1.xw, r0, s4
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex5, level=1, writemask=yz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldb r1.yz, r0, s5
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex6, level=2, writemask=yw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldb r1.yw, r0, s6
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex7, level=3, writemask=zw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldb r1.zw, r0, s7
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex8, level=4, writemask=xyz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldb r1.xyz, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex9, level=5, writemask=xyw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldb r1.xyw, r0, s9
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex10, level=0, writemask=xzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldb r1.xzw, r0, s10
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex11, level=1, writemask=yzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldb r1.yzw, r0, s11
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex12, level=2, writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldb r1.xyzw, r0, s12
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex13, level=3, writemask=x";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldp r1.x, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex14, level=4, writemask=y";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldp r1.y, r0, s14
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex15, level=5, writemask=z";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldp r1.z, r0, s15
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex0, level=0, writemask=w";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldp r1.w, r0, s0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex1, level=1, writemask=xy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldp r1.xy, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex2, level=2, writemask=xz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldp r1.xz, r0, s2
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex3, level=3, writemask=xw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldp r1.xw, r0, s3
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex4, level=4, writemask=yz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldp r1.yz, r0, s4
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex5, level=5, writemask=yw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldp r1.yw, r0, s5
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex6, level=0, writemask=zw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldp r1.zw, r0, s6
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex7, level=1, writemask=xyz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldp r1.xyz, r0, s7
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex8, level=2, writemask=xyw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldp r1.xyw, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex9, level=3, writemask=xzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldp r1.xzw, r0, s9
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex10, level=4, writemask=yzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldp r1.yzw, r0, s10
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex11, level=5, writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldp r1.xyzw, r0, s11
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex12, level=0, writemask=x";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldd r1.x, r0, s12, c0, c0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex13, level=1, writemask=y";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldd r1.y, r0, s13, c0, c0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex14, level=2, writemask=z";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldd r1.z, r0, s14, c0, c0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex15, level=3, writemask=w";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldd r1.w, r0, s15, c0, c0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex0, level=4, writemask=xy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldd r1.xy, r0, s0, c0, c0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex1, level=5, writemask=xz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldd r1.xz, r0, s1, c0, c0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex2, level=0, writemask=xw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldd r1.xw, r0, s2, c0, c0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex3, level=1, writemask=yz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldd r1.yz, r0, s3, c0, c0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex4, level=2, writemask=yw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldd r1.yw, r0, s4, c0, c0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex5, level=3, writemask=zw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldd r1.zw, r0, s5, c0, c0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex6, level=4, writemask=xyz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldd r1.xyz, r0, s6, c0, c0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex7, level=5, writemask=xyw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldd r1.xyw, r0, s7, c0, c0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex8, level=0, writemask=xzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldd r1.xzw, r0, s8, c0, c0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex9, level=1, writemask=yzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldd r1.yzw, r0, s9, c0, c0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex10, level=2, writemask=xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			mov r0.w, c0.x
			mov r1, c0.y
			texldd r1.xyzw, r0, s10, c0, c0
			mov oC0, r1
			
        };
		VertexShader = <TexTLVS>;
   }
}
