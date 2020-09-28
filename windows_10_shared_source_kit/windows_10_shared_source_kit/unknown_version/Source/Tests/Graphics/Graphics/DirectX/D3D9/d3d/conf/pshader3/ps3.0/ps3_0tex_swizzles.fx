#include "include.fx"

Technique 
< 
	String	Name = "texld : Tex0, level=0, swizzle=x";
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
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			texld oC0, r0, s0.x
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex0, level=0, swizzle=y";
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
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s0.y
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex0, level=0, swizzle=z";
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
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s0.z
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex0, level=0, swizzle=w";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s0.w
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex0, level=0, swizzle=xx";
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
			mov r0.w, c0.x
			texldd oC0, r0, s0.xx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex1, level=1, swizzle=xy";
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
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			texld oC0, r0, s1.xy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex1, level=1, swizzle=xz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s1.xz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex1, level=1, swizzle=xw";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s1.xw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex1, level=1, swizzle=yx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s1.yx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex1, level=1, swizzle=yy";
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
			mov r0.w, c0.x
			texldd oC0, r0, s1.yy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex2, level=2, swizzle=yz";
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
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			texld oC0, r0, s2.yz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex2, level=2, swizzle=yw";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s2.yw
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex2, level=2, swizzle=zx";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s2.zx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex2, level=2, swizzle=zy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s2.zy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex2, level=2, swizzle=zz";
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
			mov r0.w, c0.x
			texldd oC0, r0, s2.zz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex3, level=3, swizzle=zw";
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
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			texld oC0, r0, s3.zw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex3, level=3, swizzle=wx";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s3.wx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex3, level=3, swizzle=wy";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s3.wy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex3, level=3, swizzle=wz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s3.wz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex3, level=3, swizzle=ww";
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
			mov r0.w, c0.x
			texldd oC0, r0, s3.ww, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex4, level=4, swizzle=xxx";
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
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			texld oC0, r0, s4.xxx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex4, level=4, swizzle=xxy";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s4.xxy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex4, level=4, swizzle=xxz";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s4.xxz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex4, level=4, swizzle=xxw";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s4.xxw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex4, level=4, swizzle=xyx";
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
			mov r0.w, c0.x
			texldd oC0, r0, s4.xyx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex5, level=5, swizzle=xyy";
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
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			texld oC0, r0, s5.xyy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex5, level=5, swizzle=xyz";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s5.xyz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex5, level=5, swizzle=xyw";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s5.xyw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex5, level=5, swizzle=xzx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s5.xzx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex5, level=5, swizzle=xzy";
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
			mov r0.w, c0.x
			texldd oC0, r0, s5.xzy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex6, level=0, swizzle=xzz";
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
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			texld oC0, r0, s6.xzz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex6, level=0, swizzle=xzw";
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
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s6.xzw
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex6, level=0, swizzle=xwx";
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
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s6.xwx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex6, level=0, swizzle=xwy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s6.xwy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex6, level=0, swizzle=xwz";
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
			mov r0.w, c0.x
			texldd oC0, r0, s6.xwz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex7, level=1, swizzle=xww";
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
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			texld oC0, r0, s7.xww
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex7, level=1, swizzle=yxx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s7.yxx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex7, level=1, swizzle=yxy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s7.yxy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex7, level=1, swizzle=yxz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s7.yxz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex7, level=1, swizzle=yxw";
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
			mov r0.w, c0.x
			texldd oC0, r0, s7.yxw, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex8, level=2, swizzle=yyx";
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
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			texld oC0, r0, s8.yyx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex8, level=2, swizzle=yyy";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s8.yyy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex8, level=2, swizzle=yyz";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s8.yyz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex8, level=2, swizzle=yyw";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s8.yyw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex8, level=2, swizzle=yzx";
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
			mov r0.w, c0.x
			texldd oC0, r0, s8.yzx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex9, level=3, swizzle=yzy";
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
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			texld oC0, r0, s9.yzy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex9, level=3, swizzle=yzz";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s9.yzz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex9, level=3, swizzle=yzw";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s9.yzw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex9, level=3, swizzle=ywx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s9.ywx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex9, level=3, swizzle=ywy";
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
			mov r0.w, c0.x
			texldd oC0, r0, s9.ywy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex10, level=4, swizzle=ywz";
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
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			texld oC0, r0, s10.ywz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex10, level=4, swizzle=yww";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s10.yww
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex10, level=4, swizzle=zxx";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s10.zxx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex10, level=4, swizzle=zxy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s10.zxy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex10, level=4, swizzle=zxz";
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
			mov r0.w, c0.x
			texldd oC0, r0, s10.zxz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex11, level=5, swizzle=zxw";
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
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			texld oC0, r0, s11.zxw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex11, level=5, swizzle=zyx";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s11.zyx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex11, level=5, swizzle=zyy";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s11.zyy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex11, level=5, swizzle=zyz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s11.zyz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex11, level=5, swizzle=zyw";
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
			mov r0.w, c0.x
			texldd oC0, r0, s11.zyw, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex12, level=0, swizzle=zzx";
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
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			texld oC0, r0, s12.zzx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex12, level=0, swizzle=zzy";
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
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s12.zzy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex12, level=0, swizzle=zzz";
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
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s12.zzz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex12, level=0, swizzle=zzw";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s12.zzw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex12, level=0, swizzle=zwx";
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
			texldd oC0, r0, s12.zwx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex13, level=1, swizzle=zwy";
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
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			texld oC0, r0, s13.zwy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex13, level=1, swizzle=zwz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s13.zwz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex13, level=1, swizzle=zww";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s13.zww
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex13, level=1, swizzle=wxx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s13.wxx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex13, level=1, swizzle=wxy";
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
			texldd oC0, r0, s13.wxy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex14, level=2, swizzle=wxz";
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
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			texld oC0, r0, s14.wxz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex14, level=2, swizzle=wxw";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s14.wxw
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex14, level=2, swizzle=wyx";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s14.wyx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex14, level=2, swizzle=wyy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s14.wyy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex14, level=2, swizzle=wyz";
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
			texldd oC0, r0, s14.wyz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex15, level=3, swizzle=wyw";
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
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			texld oC0, r0, s15.wyw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex15, level=3, swizzle=wzx";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s15.wzx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex15, level=3, swizzle=wzy";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s15.wzy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex15, level=3, swizzle=wzz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s15.wzz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex15, level=3, swizzle=wzw";
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
			texldd oC0, r0, s15.wzw, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex0, level=4, swizzle=wwx";
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
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			texld oC0, r0, s0.wwx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex0, level=4, swizzle=wwy";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s0.wwy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex0, level=4, swizzle=wwz";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s0.wwz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex0, level=4, swizzle=www";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s0.www
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex0, level=4, swizzle=xxxx";
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
			texldd oC0, r0, s0.xxxx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex1, level=5, swizzle=xxxy";
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
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			texld oC0, r0, s1.xxxy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex1, level=5, swizzle=xxxz";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s1.xxxz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex1, level=5, swizzle=xxxw";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s1.xxxw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex1, level=5, swizzle=xxyx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s1.xxyx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex1, level=5, swizzle=xxyy";
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
			texldd oC0, r0, s1.xxyy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex2, level=0, swizzle=xxyz";
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
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			texld oC0, r0, s2.xxyz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex2, level=0, swizzle=xxyw";
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
			texldl oC0, r0, s2.xxyw
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex2, level=0, swizzle=xxzx";
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
			texldb oC0, r0, s2.xxzx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex2, level=0, swizzle=xxzy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s2.xxzy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex2, level=0, swizzle=xxzz";
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
			texldd oC0, r0, s2.xxzz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex3, level=1, swizzle=xxzw";
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
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			texld oC0, r0, s3.xxzw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex3, level=1, swizzle=xxwx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s3.xxwx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex3, level=1, swizzle=xxwy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s3.xxwy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex3, level=1, swizzle=xxwz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s3.xxwz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex3, level=1, swizzle=xxww";
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
			texldd oC0, r0, s3.xxww, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex4, level=2, swizzle=xyxx";
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
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			texld oC0, r0, s4.xyxx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex4, level=2, swizzle=xyxy";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s4.xyxy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex4, level=2, swizzle=xyxz";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s4.xyxz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex4, level=2, swizzle=xyxw";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s4.xyxw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex4, level=2, swizzle=xyyx";
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
			texldd oC0, r0, s4.xyyx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex5, level=3, swizzle=xyyy";
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
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			texld oC0, r0, s5.xyyy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex5, level=3, swizzle=xyyz";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s5.xyyz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex5, level=3, swizzle=xyyw";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s5.xyyw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex5, level=3, swizzle=xyzx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s5.xyzx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex5, level=3, swizzle=xyzy";
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
			texldd oC0, r0, s5.xyzy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex6, level=4, swizzle=xyzz";
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
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			texld oC0, r0, s6.xyzz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex6, level=4, swizzle=xyzw";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s6.xyzw
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex6, level=4, swizzle=xywx";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s6.xywx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex6, level=4, swizzle=xywy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s6.xywy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex6, level=4, swizzle=xywz";
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
			texldd oC0, r0, s6.xywz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex7, level=5, swizzle=xyww";
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
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			texld oC0, r0, s7.xyww
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex7, level=5, swizzle=xzxx";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s7.xzxx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex7, level=5, swizzle=xzxy";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s7.xzxy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex7, level=5, swizzle=xzxz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s7.xzxz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex7, level=5, swizzle=xzxw";
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
			texldd oC0, r0, s7.xzxw, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex8, level=0, swizzle=xzyx";
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
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			texld oC0, r0, s8.xzyx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex8, level=0, swizzle=xzyy";
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
			texldl oC0, r0, s8.xzyy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex8, level=0, swizzle=xzyz";
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
			texldb oC0, r0, s8.xzyz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex8, level=0, swizzle=xzyw";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s8.xzyw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex8, level=0, swizzle=xzzx";
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
			texldd oC0, r0, s8.xzzx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex9, level=1, swizzle=xzzy";
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
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			texld oC0, r0, s9.xzzy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex9, level=1, swizzle=xzzz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s9.xzzz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex9, level=1, swizzle=xzzw";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s9.xzzw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex9, level=1, swizzle=xzwx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s9.xzwx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex9, level=1, swizzle=xzwy";
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
			texldd oC0, r0, s9.xzwy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex10, level=2, swizzle=xzwz";
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
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			texld oC0, r0, s10.xzwz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex10, level=2, swizzle=xzww";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s10.xzww
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex10, level=2, swizzle=xwxx";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s10.xwxx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex10, level=2, swizzle=xwxy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s10.xwxy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex10, level=2, swizzle=xwxz";
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
			texldd oC0, r0, s10.xwxz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex11, level=3, swizzle=xwxw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			texld oC0, r0, s11.xwxw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex11, level=3, swizzle=xwyx";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s11.xwyx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex11, level=3, swizzle=xwyy";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s11.xwyy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex11, level=3, swizzle=xwyz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s11.xwyz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex11, level=3, swizzle=xwyw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldd oC0, r0, s11.xwyw, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex12, level=4, swizzle=xwzx";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			texld oC0, r0, s12.xwzx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex12, level=4, swizzle=xwzy";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s12.xwzy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex12, level=4, swizzle=xwzz";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s12.xwzz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex12, level=4, swizzle=xwzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s12.xwzw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex12, level=4, swizzle=xwwx";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
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
			texldd oC0, r0, s12.xwwx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex13, level=5, swizzle=xwwy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			texld oC0, r0, s13.xwwy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex13, level=5, swizzle=xwwz";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s13.xwwz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex13, level=5, swizzle=xwww";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s13.xwww
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex13, level=5, swizzle=yxxx";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s13.yxxx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex13, level=5, swizzle=yxxy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 5;
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
			texldd oC0, r0, s13.yxxy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex14, level=0, swizzle=yxxz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			texld oC0, r0, s14.yxxz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex14, level=0, swizzle=yxxw";
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
			texldl oC0, r0, s14.yxxw
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex14, level=0, swizzle=yxyx";
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
			texldb oC0, r0, s14.yxyx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex14, level=0, swizzle=yxyy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s14.yxyy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex14, level=0, swizzle=yxyz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
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
			texldd oC0, r0, s14.yxyz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex15, level=1, swizzle=yxyw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			texld oC0, r0, s15.yxyw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex15, level=1, swizzle=yxzx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s15.yxzx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex15, level=1, swizzle=yxzy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s15.yxzy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex15, level=1, swizzle=yxzz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s15.yxzz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex15, level=1, swizzle=yxzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[15] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 1;
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
			texldd oC0, r0, s15.yxzw, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex0, level=2, swizzle=yxwx";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			texld oC0, r0, s0.yxwx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex0, level=2, swizzle=yxwy";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s0.yxwy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex0, level=2, swizzle=yxwz";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s0.yxwz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex0, level=2, swizzle=yxww";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s0.yxww
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex0, level=2, swizzle=yyxx";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 2;
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
			texldd oC0, r0, s0.yyxx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex1, level=3, swizzle=yyxy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			texld oC0, r0, s1.yyxy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex1, level=3, swizzle=yyxz";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s1.yyxz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex1, level=3, swizzle=yyxw";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s1.yyxw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex1, level=3, swizzle=yyyx";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s1.yyyx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex1, level=3, swizzle=yyyy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 3;
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
			texldd oC0, r0, s1.yyyy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex2, level=4, swizzle=yyyz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			texld oC0, r0, s2.yyyz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex2, level=4, swizzle=yyyw";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s2.yyyw
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex2, level=4, swizzle=yyzx";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s2.yyzx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex2, level=4, swizzle=yyzy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s2.yyzy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex2, level=4, swizzle=yyzz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 4;
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
			texldd oC0, r0, s2.yyzz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex3, level=5, swizzle=yyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			texld oC0, r0, s3.yyzw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex3, level=5, swizzle=yywx";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s3.yywx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex3, level=5, swizzle=yywy";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s3.yywy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex3, level=5, swizzle=yywz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s3.yywz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex3, level=5, swizzle=yyww";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 5;
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
			texldd oC0, r0, s3.yyww, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex4, level=0, swizzle=yzxx";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			texld oC0, r0, s4.yzxx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex4, level=0, swizzle=yzxy";
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
			texldl oC0, r0, s4.yzxy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex4, level=0, swizzle=yzxz";
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
			texldb oC0, r0, s4.yzxz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex4, level=0, swizzle=yzxw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s4.yzxw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex4, level=0, swizzle=yzyx";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
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
			texldd oC0, r0, s4.yzyx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex5, level=1, swizzle=yzyy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			texld oC0, r0, s5.yzyy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex5, level=1, swizzle=yzyz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s5.yzyz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex5, level=1, swizzle=yzyw";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s5.yzyw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex5, level=1, swizzle=yzzx";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s5.yzzx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex5, level=1, swizzle=yzzy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
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
			texldd oC0, r0, s5.yzzy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex6, level=2, swizzle=yzzz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			texld oC0, r0, s6.yzzz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex6, level=2, swizzle=yzzw";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s6.yzzw
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex6, level=2, swizzle=yzwx";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s6.yzwx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex6, level=2, swizzle=yzwy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s6.yzwy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex6, level=2, swizzle=yzwz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
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
			texldd oC0, r0, s6.yzwz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex7, level=3, swizzle=yzww";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			texld oC0, r0, s7.yzww
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex7, level=3, swizzle=ywxx";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s7.ywxx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex7, level=3, swizzle=ywxy";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s7.ywxy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex7, level=3, swizzle=ywxz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s7.ywxz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex7, level=3, swizzle=ywxw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
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
			texldd oC0, r0, s7.ywxw, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex8, level=4, swizzle=ywyx";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			texld oC0, r0, s8.ywyx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex8, level=4, swizzle=ywyy";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s8.ywyy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex8, level=4, swizzle=ywyz";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s8.ywyz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex8, level=4, swizzle=ywyw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s8.ywyw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex8, level=4, swizzle=ywzx";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 4;
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
			texldd oC0, r0, s8.ywzx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex9, level=5, swizzle=ywzy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			texld oC0, r0, s9.ywzy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex9, level=5, swizzle=ywzz";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s9.ywzz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex9, level=5, swizzle=ywzw";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s9.ywzw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex9, level=5, swizzle=ywwx";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s9.ywwx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex9, level=5, swizzle=ywwy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
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
			texldd oC0, r0, s9.ywwy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex10, level=0, swizzle=ywwz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			texld oC0, r0, s10.ywwz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex10, level=0, swizzle=ywww";
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
			texldl oC0, r0, s10.ywww
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex10, level=0, swizzle=zxxx";
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
			texldb oC0, r0, s10.zxxx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex10, level=0, swizzle=zxxy";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s10.zxxy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex10, level=0, swizzle=zxxz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
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
			texldd oC0, r0, s10.zxxz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex11, level=1, swizzle=zxxw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			texld oC0, r0, s11.zxxw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex11, level=1, swizzle=zxyx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s11.zxyx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex11, level=1, swizzle=zxyy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s11.zxyy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex11, level=1, swizzle=zxyz";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s11.zxyz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex11, level=1, swizzle=zxyw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldd oC0, r0, s11.zxyw, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex12, level=2, swizzle=zxzx";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			texld oC0, r0, s12.zxzx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex12, level=2, swizzle=zxzy";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s12.zxzy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex12, level=2, swizzle=zxzz";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s12.zxzz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex12, level=2, swizzle=zxzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s12.zxzw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex12, level=2, swizzle=zxwx";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
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
			texldd oC0, r0, s12.zxwx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex13, level=3, swizzle=zxwy";
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
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			texld oC0, r0, s13.zxwy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex13, level=3, swizzle=zxwz";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s13.zxwz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex13, level=3, swizzle=zxww";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s13.zxww
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex13, level=3, swizzle=zyxx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s13.zyxx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex13, level=3, swizzle=zyxy";
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
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldd oC0, r0, s13.zyxy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex14, level=4, swizzle=zyxz";
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
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			texld oC0, r0, s14.zyxz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex14, level=4, swizzle=zyxw";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s14.zyxw
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex14, level=4, swizzle=zyyx";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s14.zyyx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex14, level=4, swizzle=zyyy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s14.zyyy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex14, level=4, swizzle=zyyz";
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
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldd oC0, r0, s14.zyyz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex15, level=5, swizzle=zyyw";
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
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			texld oC0, r0, s15.zyyw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex15, level=5, swizzle=zyzx";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s15.zyzx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex15, level=5, swizzle=zyzy";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s15.zyzy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex15, level=5, swizzle=zyzz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s15.zyzz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex15, level=5, swizzle=zyzw";
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
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldd oC0, r0, s15.zyzw, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex0, level=0, swizzle=zywx";
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
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			texld oC0, r0, s0.zywx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex0, level=0, swizzle=zywy";
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
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s0.zywy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex0, level=0, swizzle=zywz";
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
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s0.zywz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex0, level=0, swizzle=zyww";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s0.zyww
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex0, level=0, swizzle=zzxx";
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
			mov r0.w, c0.x
			texldd oC0, r0, s0.zzxx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex1, level=1, swizzle=zzxy";
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
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			texld oC0, r0, s1.zzxy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex1, level=1, swizzle=zzxz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s1.zzxz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex1, level=1, swizzle=zzxw";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s1.zzxw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex1, level=1, swizzle=zzyx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s1.zzyx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex1, level=1, swizzle=zzyy";
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
			mov r0.w, c0.x
			texldd oC0, r0, s1.zzyy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex2, level=2, swizzle=zzyz";
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
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			texld oC0, r0, s2.zzyz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex2, level=2, swizzle=zzyw";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s2.zzyw
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex2, level=2, swizzle=zzzx";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s2.zzzx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex2, level=2, swizzle=zzzy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s2.zzzy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex2, level=2, swizzle=zzzz";
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
			mov r0.w, c0.x
			texldd oC0, r0, s2.zzzz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex3, level=3, swizzle=zzzw";
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
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			texld oC0, r0, s3.zzzw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex3, level=3, swizzle=zzwx";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s3.zzwx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex3, level=3, swizzle=zzwy";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s3.zzwy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex3, level=3, swizzle=zzwz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s3.zzwz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex3, level=3, swizzle=zzww";
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
			mov r0.w, c0.x
			texldd oC0, r0, s3.zzww, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex4, level=4, swizzle=zwxx";
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
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			texld oC0, r0, s4.zwxx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex4, level=4, swizzle=zwxy";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s4.zwxy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex4, level=4, swizzle=zwxz";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s4.zwxz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex4, level=4, swizzle=zwxw";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s4.zwxw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex4, level=4, swizzle=zwyx";
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
			mov r0.w, c0.x
			texldd oC0, r0, s4.zwyx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex5, level=5, swizzle=zwyy";
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
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			texld oC0, r0, s5.zwyy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex5, level=5, swizzle=zwyz";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s5.zwyz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex5, level=5, swizzle=zwyw";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s5.zwyw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex5, level=5, swizzle=zwzx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s5.zwzx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex5, level=5, swizzle=zwzy";
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
			mov r0.w, c0.x
			texldd oC0, r0, s5.zwzy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex6, level=0, swizzle=zwzz";
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
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			texld oC0, r0, s6.zwzz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex6, level=0, swizzle=zwzw";
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
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s6.zwzw
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex6, level=0, swizzle=zwwx";
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
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s6.zwwx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex6, level=0, swizzle=zwwy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s6.zwwy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex6, level=0, swizzle=zwwz";
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
			mov r0.w, c0.x
			texldd oC0, r0, s6.zwwz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex7, level=1, swizzle=zwww";
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
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			texld oC0, r0, s7.zwww
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex7, level=1, swizzle=wxxx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s7.wxxx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex7, level=1, swizzle=wxxy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s7.wxxy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex7, level=1, swizzle=wxxz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s7.wxxz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex7, level=1, swizzle=wxxw";
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
			mov r0.w, c0.x
			texldd oC0, r0, s7.wxxw, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex8, level=2, swizzle=wxyx";
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
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			texld oC0, r0, s8.wxyx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex8, level=2, swizzle=wxyy";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s8.wxyy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex8, level=2, swizzle=wxyz";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s8.wxyz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex8, level=2, swizzle=wxyw";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s8.wxyw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex8, level=2, swizzle=wxzx";
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
			mov r0.w, c0.x
			texldd oC0, r0, s8.wxzx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex9, level=3, swizzle=wxzy";
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
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			texld oC0, r0, s9.wxzy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex9, level=3, swizzle=wxzz";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s9.wxzz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex9, level=3, swizzle=wxzw";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s9.wxzw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex9, level=3, swizzle=wxwx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s9.wxwx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex9, level=3, swizzle=wxwy";
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
			mov r0.w, c0.x
			texldd oC0, r0, s9.wxwy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex10, level=4, swizzle=wxwz";
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
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			texld oC0, r0, s10.wxwz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex10, level=4, swizzle=wxww";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s10.wxww
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex10, level=4, swizzle=wyxx";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s10.wyxx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex10, level=4, swizzle=wyxy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s10.wyxy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex10, level=4, swizzle=wyxz";
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
			mov r0.w, c0.x
			texldd oC0, r0, s10.wyxz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex11, level=5, swizzle=wyxw";
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
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			texld oC0, r0, s11.wyxw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex11, level=5, swizzle=wyyx";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s11.wyyx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex11, level=5, swizzle=wyyy";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s11.wyyy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex11, level=5, swizzle=wyyz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s11.wyyz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex11, level=5, swizzle=wyyw";
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
			mov r0.w, c0.x
			texldd oC0, r0, s11.wyyw, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex12, level=0, swizzle=wyzx";
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
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			texld oC0, r0, s12.wyzx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex12, level=0, swizzle=wyzy";
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
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s12.wyzy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex12, level=0, swizzle=wyzz";
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
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s12.wyzz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex12, level=0, swizzle=wyzw";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s12.wyzw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex12, level=0, swizzle=wywx";
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
			texldd oC0, r0, s12.wywx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex13, level=1, swizzle=wywy";
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
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			texld oC0, r0, s13.wywy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex13, level=1, swizzle=wywz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s13.wywz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex13, level=1, swizzle=wyww";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s13.wyww
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex13, level=1, swizzle=wzxx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s13.wzxx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex13, level=1, swizzle=wzxy";
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
			texldd oC0, r0, s13.wzxy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex14, level=2, swizzle=wzxz";
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
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			texld oC0, r0, s14.wzxz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex14, level=2, swizzle=wzxw";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s14.wzxw
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex14, level=2, swizzle=wzyx";
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
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s14.wzyx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex14, level=2, swizzle=wzyy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s14.wzyy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex14, level=2, swizzle=wzyz";
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
			texldd oC0, r0, s14.wzyz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex15, level=3, swizzle=wzyw";
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
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			texld oC0, r0, s15.wzyw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex15, level=3, swizzle=wzzx";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s15.wzzx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex15, level=3, swizzle=wzzy";
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
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s15.wzzy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex15, level=3, swizzle=wzzz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s15
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s15.wzzz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex15, level=3, swizzle=wzzw";
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
			texldd oC0, r0, s15.wzzw, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex0, level=4, swizzle=wzwx";
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
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			texld oC0, r0, s0.wzwx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex0, level=4, swizzle=wzwy";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s0.wzwy
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex0, level=4, swizzle=wzwz";
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
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s0.wzwz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex0, level=4, swizzle=wzww";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s0.wzww
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex0, level=4, swizzle=wwxx";
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
			texldd oC0, r0, s0.wwxx, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex1, level=5, swizzle=wwxy";
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
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			texld oC0, r0, s1.wwxy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex1, level=5, swizzle=wwxz";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s1.wwxz
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex1, level=5, swizzle=wwxw";
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
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s1.wwxw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex1, level=5, swizzle=wwyx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s1.wwyx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex1, level=5, swizzle=wwyy";
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
			texldd oC0, r0, s1.wwyy, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex2, level=0, swizzle=wwyz";
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
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			texld oC0, r0, s2.wwyz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex2, level=0, swizzle=wwyw";
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
			texldl oC0, r0, s2.wwyw
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex2, level=0, swizzle=wwzx";
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
			texldb oC0, r0, s2.wwzx
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex2, level=0, swizzle=wwzy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s2.wwzy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex2, level=0, swizzle=wwzz";
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
			texldd oC0, r0, s2.wwzz, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Tex3, level=1, swizzle=wwzw";
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
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			texld oC0, r0, s3.wwzw
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Tex3, level=1, swizzle=wwwx";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldl oC0, r0, s3.wwwx
			
        };
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Tex3, level=1, swizzle=wwwy";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldb oC0, r0, s3.wwwy
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Tex3, level=1, swizzle=wwwz";
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
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp oC0, r0, s3.wwwz
			
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Tex3, level=1, swizzle=wwww";
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
			texldd oC0, r0, s3.wwww, c0, c0
			
        };
		VertexShader = <TexTLVS>;
   }
}
