#include "include.fx"

Technique 
< 
	String	Name = "texld : Vol0, level=0, swizzle=x";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			texld oC0, r0, s0.x
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol0, level=0, swizzle=y";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s0.y
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol0, level=0, swizzle=z";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s0.z
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol0, level=0, swizzle=w";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s0.w
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol0, level=0, swizzle=xx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s0.xx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol1, level=1, swizzle=xy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			texld oC0, r0, s1.xy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol1, level=1, swizzle=xz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s1.xz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol1, level=1, swizzle=xw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s1.xw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol1, level=1, swizzle=yx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s1.yx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol1, level=1, swizzle=yy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s1.yy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol2, level=2, swizzle=yz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			texld oC0, r0, s2.yz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol2, level=2, swizzle=yw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s2.yw
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol2, level=2, swizzle=zx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s2.zx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol2, level=2, swizzle=zy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s2.zy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol2, level=2, swizzle=zz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s2.zz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol3, level=3, swizzle=zw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			texld oC0, r0, s3.zw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol3, level=3, swizzle=wx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s3.wx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol3, level=3, swizzle=wy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s3.wy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol3, level=3, swizzle=wz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s3.wz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol3, level=3, swizzle=ww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s3.ww, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol4, level=4, swizzle=xxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			texld oC0, r0, s4.xxx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol4, level=4, swizzle=xxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s4.xxy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol4, level=4, swizzle=xxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s4.xxz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol4, level=4, swizzle=xxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s4.xxw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol4, level=4, swizzle=xyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s4.xyx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol5, level=5, swizzle=xyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			texld oC0, r0, s5.xyy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol5, level=5, swizzle=xyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s5.xyz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol5, level=5, swizzle=xyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s5.xyw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol5, level=5, swizzle=xzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s5.xzx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol5, level=5, swizzle=xzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s5.xzy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol6, level=0, swizzle=xzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			texld oC0, r0, s6.xzz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol6, level=0, swizzle=xzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s6.xzw
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol6, level=0, swizzle=xwx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s6.xwx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol6, level=0, swizzle=xwy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s6.xwy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol6, level=0, swizzle=xwz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s6.xwz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol7, level=1, swizzle=xww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			texld oC0, r0, s7.xww
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol7, level=1, swizzle=yxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s7.yxx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol7, level=1, swizzle=yxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s7.yxy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol7, level=1, swizzle=yxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s7.yxz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol7, level=1, swizzle=yxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s7.yxw, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol8, level=2, swizzle=yyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			texld oC0, r0, s8.yyx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol8, level=2, swizzle=yyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s8.yyy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol8, level=2, swizzle=yyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s8.yyz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol8, level=2, swizzle=yyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s8.yyw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol8, level=2, swizzle=yzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s8.yzx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol9, level=3, swizzle=yzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			texld oC0, r0, s9.yzy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol9, level=3, swizzle=yzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s9.yzz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol9, level=3, swizzle=yzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s9.yzw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol9, level=3, swizzle=ywx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s9.ywx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol9, level=3, swizzle=ywy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s9.ywy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol10, level=4, swizzle=ywz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			texld oC0, r0, s10.ywz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol10, level=4, swizzle=yww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s10.yww
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol10, level=4, swizzle=zxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s10.zxx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol10, level=4, swizzle=zxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s10.zxy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol10, level=4, swizzle=zxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s10.zxz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol11, level=5, swizzle=zxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			texld oC0, r0, s11.zxw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol11, level=5, swizzle=zyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s11.zyx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol11, level=5, swizzle=zyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s11.zyy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol11, level=5, swizzle=zyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s11.zyz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol11, level=5, swizzle=zyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s11.zyw, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol12, level=0, swizzle=zzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			texld oC0, r0, s12.zzx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol12, level=0, swizzle=zzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s12.zzy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol12, level=0, swizzle=zzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s12.zzz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol12, level=0, swizzle=zzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s12.zzw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol12, level=0, swizzle=zwx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s12.zwx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol13, level=1, swizzle=zwy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			texld oC0, r0, s13.zwy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol13, level=1, swizzle=zwz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s13.zwz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol13, level=1, swizzle=zww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s13.zww
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol13, level=1, swizzle=wxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s13.wxx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol13, level=1, swizzle=wxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s13.wxy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol14, level=2, swizzle=wxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			texld oC0, r0, s14.wxz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol14, level=2, swizzle=wxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s14.wxw
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol14, level=2, swizzle=wyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s14.wyx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol14, level=2, swizzle=wyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s14.wyy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol14, level=2, swizzle=wyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s14.wyz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol15, level=3, swizzle=wyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			texld oC0, r0, s15.wyw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol15, level=3, swizzle=wzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s15.wzx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol15, level=3, swizzle=wzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s15.wzy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol15, level=3, swizzle=wzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s15.wzz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol15, level=3, swizzle=wzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s15.wzw, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol0, level=4, swizzle=wwx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			texld oC0, r0, s0.wwx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol0, level=4, swizzle=wwy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s0.wwy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol0, level=4, swizzle=wwz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s0.wwz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol0, level=4, swizzle=www";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s0.www
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol0, level=4, swizzle=xxxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s0.xxxx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol1, level=5, swizzle=xxxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			texld oC0, r0, s1.xxxy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol1, level=5, swizzle=xxxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s1.xxxz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol1, level=5, swizzle=xxxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s1.xxxw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol1, level=5, swizzle=xxyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s1.xxyx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol1, level=5, swizzle=xxyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s1.xxyy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol2, level=0, swizzle=xxyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			texld oC0, r0, s2.xxyz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol2, level=0, swizzle=xxyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s2.xxyw
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol2, level=0, swizzle=xxzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s2.xxzx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol2, level=0, swizzle=xxzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s2.xxzy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol2, level=0, swizzle=xxzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s2.xxzz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol3, level=1, swizzle=xxzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			texld oC0, r0, s3.xxzw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol3, level=1, swizzle=xxwx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s3.xxwx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol3, level=1, swizzle=xxwy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s3.xxwy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol3, level=1, swizzle=xxwz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s3.xxwz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol3, level=1, swizzle=xxww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s3.xxww, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol4, level=2, swizzle=xyxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			texld oC0, r0, s4.xyxx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol4, level=2, swizzle=xyxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s4.xyxy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol4, level=2, swizzle=xyxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s4.xyxz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol4, level=2, swizzle=xyxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s4.xyxw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol4, level=2, swizzle=xyyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s4.xyyx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol5, level=3, swizzle=xyyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			texld oC0, r0, s5.xyyy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol5, level=3, swizzle=xyyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s5.xyyz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol5, level=3, swizzle=xyyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s5.xyyw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol5, level=3, swizzle=xyzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s5.xyzx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol5, level=3, swizzle=xyzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s5.xyzy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol6, level=4, swizzle=xyzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			texld oC0, r0, s6.xyzz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol6, level=4, swizzle=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s6.xyzw
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol6, level=4, swizzle=xywx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s6.xywx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol6, level=4, swizzle=xywy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s6.xywy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol6, level=4, swizzle=xywz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s6.xywz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol7, level=5, swizzle=xyww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			texld oC0, r0, s7.xyww
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol7, level=5, swizzle=xzxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s7.xzxx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol7, level=5, swizzle=xzxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s7.xzxy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol7, level=5, swizzle=xzxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s7.xzxz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol7, level=5, swizzle=xzxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s7.xzxw, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol8, level=0, swizzle=xzyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			texld oC0, r0, s8.xzyx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol8, level=0, swizzle=xzyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s8.xzyy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol8, level=0, swizzle=xzyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s8.xzyz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol8, level=0, swizzle=xzyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s8.xzyw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol8, level=0, swizzle=xzzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s8.xzzx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol9, level=1, swizzle=xzzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			texld oC0, r0, s9.xzzy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol9, level=1, swizzle=xzzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s9.xzzz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol9, level=1, swizzle=xzzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s9.xzzw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol9, level=1, swizzle=xzwx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s9.xzwx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol9, level=1, swizzle=xzwy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s9.xzwy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol10, level=2, swizzle=xzwz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			texld oC0, r0, s10.xzwz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol10, level=2, swizzle=xzww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s10.xzww
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol10, level=2, swizzle=xwxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s10.xwxx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol10, level=2, swizzle=xwxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s10.xwxy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol10, level=2, swizzle=xwxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s10.xwxz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol11, level=3, swizzle=xwxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			texld oC0, r0, s11.xwxw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol11, level=3, swizzle=xwyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s11.xwyx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol11, level=3, swizzle=xwyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s11.xwyy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol11, level=3, swizzle=xwyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s11.xwyz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol11, level=3, swizzle=xwyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s11.xwyw, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol12, level=4, swizzle=xwzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			texld oC0, r0, s12.xwzx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol12, level=4, swizzle=xwzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s12.xwzy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol12, level=4, swizzle=xwzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s12.xwzz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol12, level=4, swizzle=xwzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s12.xwzw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol12, level=4, swizzle=xwwx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s12.xwwx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol13, level=5, swizzle=xwwy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			texld oC0, r0, s13.xwwy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol13, level=5, swizzle=xwwz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s13.xwwz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol13, level=5, swizzle=xwww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s13.xwww
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol13, level=5, swizzle=yxxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s13.yxxx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol13, level=5, swizzle=yxxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s13.yxxy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol14, level=0, swizzle=yxxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			texld oC0, r0, s14.yxxz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol14, level=0, swizzle=yxxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s14.yxxw
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol14, level=0, swizzle=yxyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s14.yxyx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol14, level=0, swizzle=yxyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s14.yxyy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol14, level=0, swizzle=yxyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s14.yxyz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol15, level=1, swizzle=yxyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			texld oC0, r0, s15.yxyw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol15, level=1, swizzle=yxzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s15.yxzx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol15, level=1, swizzle=yxzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s15.yxzy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol15, level=1, swizzle=yxzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s15.yxzz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol15, level=1, swizzle=yxzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s15.yxzw, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol0, level=2, swizzle=yxwx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			texld oC0, r0, s0.yxwx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol0, level=2, swizzle=yxwy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s0.yxwy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol0, level=2, swizzle=yxwz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s0.yxwz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol0, level=2, swizzle=yxww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s0.yxww
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol0, level=2, swizzle=yyxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s0.yyxx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol1, level=3, swizzle=yyxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			texld oC0, r0, s1.yyxy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol1, level=3, swizzle=yyxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s1.yyxz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol1, level=3, swizzle=yyxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s1.yyxw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol1, level=3, swizzle=yyyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s1.yyyx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol1, level=3, swizzle=yyyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s1.yyyy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol2, level=4, swizzle=yyyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			texld oC0, r0, s2.yyyz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol2, level=4, swizzle=yyyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s2.yyyw
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol2, level=4, swizzle=yyzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s2.yyzx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol2, level=4, swizzle=yyzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s2.yyzy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol2, level=4, swizzle=yyzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s2.yyzz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol3, level=5, swizzle=yyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			texld oC0, r0, s3.yyzw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol3, level=5, swizzle=yywx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s3.yywx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol3, level=5, swizzle=yywy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s3.yywy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol3, level=5, swizzle=yywz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s3.yywz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol3, level=5, swizzle=yyww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s3.yyww, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol4, level=0, swizzle=yzxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			texld oC0, r0, s4.yzxx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol4, level=0, swizzle=yzxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s4.yzxy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol4, level=0, swizzle=yzxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s4.yzxz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol4, level=0, swizzle=yzxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s4.yzxw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol4, level=0, swizzle=yzyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s4.yzyx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol5, level=1, swizzle=yzyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			texld oC0, r0, s5.yzyy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol5, level=1, swizzle=yzyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s5.yzyz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol5, level=1, swizzle=yzyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s5.yzyw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol5, level=1, swizzle=yzzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s5.yzzx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol5, level=1, swizzle=yzzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s5.yzzy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol6, level=2, swizzle=yzzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			texld oC0, r0, s6.yzzz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol6, level=2, swizzle=yzzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s6.yzzw
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol6, level=2, swizzle=yzwx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s6.yzwx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol6, level=2, swizzle=yzwy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s6.yzwy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol6, level=2, swizzle=yzwz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s6.yzwz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol7, level=3, swizzle=yzww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			texld oC0, r0, s7.yzww
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol7, level=3, swizzle=ywxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s7.ywxx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol7, level=3, swizzle=ywxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s7.ywxy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol7, level=3, swizzle=ywxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s7.ywxz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol7, level=3, swizzle=ywxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s7.ywxw, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol8, level=4, swizzle=ywyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			texld oC0, r0, s8.ywyx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol8, level=4, swizzle=ywyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s8.ywyy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol8, level=4, swizzle=ywyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s8.ywyz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol8, level=4, swizzle=ywyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s8.ywyw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol8, level=4, swizzle=ywzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s8.ywzx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol9, level=5, swizzle=ywzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			texld oC0, r0, s9.ywzy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol9, level=5, swizzle=ywzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s9.ywzz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol9, level=5, swizzle=ywzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s9.ywzw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol9, level=5, swizzle=ywwx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s9.ywwx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol9, level=5, swizzle=ywwy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s9.ywwy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol10, level=0, swizzle=ywwz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			texld oC0, r0, s10.ywwz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol10, level=0, swizzle=ywww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s10.ywww
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol10, level=0, swizzle=zxxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s10.zxxx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol10, level=0, swizzle=zxxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s10.zxxy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol10, level=0, swizzle=zxxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s10.zxxz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol11, level=1, swizzle=zxxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			texld oC0, r0, s11.zxxw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol11, level=1, swizzle=zxyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s11.zxyx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol11, level=1, swizzle=zxyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s11.zxyy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol11, level=1, swizzle=zxyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s11.zxyz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol11, level=1, swizzle=zxyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s11.zxyw, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol12, level=2, swizzle=zxzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			texld oC0, r0, s12.zxzx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol12, level=2, swizzle=zxzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s12.zxzy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol12, level=2, swizzle=zxzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s12.zxzz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol12, level=2, swizzle=zxzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s12.zxzw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol12, level=2, swizzle=zxwx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s12.zxwx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol13, level=3, swizzle=zxwy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			texld oC0, r0, s13.zxwy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol13, level=3, swizzle=zxwz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s13.zxwz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol13, level=3, swizzle=zxww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s13.zxww
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol13, level=3, swizzle=zyxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s13.zyxx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol13, level=3, swizzle=zyxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s13.zyxy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol14, level=4, swizzle=zyxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			texld oC0, r0, s14.zyxz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol14, level=4, swizzle=zyxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s14.zyxw
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol14, level=4, swizzle=zyyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s14.zyyx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol14, level=4, swizzle=zyyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s14.zyyy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol14, level=4, swizzle=zyyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s14.zyyz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol15, level=5, swizzle=zyyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			texld oC0, r0, s15.zyyw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol15, level=5, swizzle=zyzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s15.zyzx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol15, level=5, swizzle=zyzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s15.zyzy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol15, level=5, swizzle=zyzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s15.zyzz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol15, level=5, swizzle=zyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s15.zyzw, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol0, level=0, swizzle=zywx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			texld oC0, r0, s0.zywx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol0, level=0, swizzle=zywy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s0.zywy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol0, level=0, swizzle=zywz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s0.zywz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol0, level=0, swizzle=zyww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s0.zyww
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol0, level=0, swizzle=zzxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s0.zzxx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol1, level=1, swizzle=zzxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			texld oC0, r0, s1.zzxy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol1, level=1, swizzle=zzxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s1.zzxz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol1, level=1, swizzle=zzxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s1.zzxw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol1, level=1, swizzle=zzyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s1.zzyx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol1, level=1, swizzle=zzyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s1.zzyy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol2, level=2, swizzle=zzyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			texld oC0, r0, s2.zzyz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol2, level=2, swizzle=zzyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s2.zzyw
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol2, level=2, swizzle=zzzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s2.zzzx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol2, level=2, swizzle=zzzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s2.zzzy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol2, level=2, swizzle=zzzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s2.zzzz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol3, level=3, swizzle=zzzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			texld oC0, r0, s3.zzzw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol3, level=3, swizzle=zzwx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s3.zzwx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol3, level=3, swizzle=zzwy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s3.zzwy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol3, level=3, swizzle=zzwz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s3.zzwz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol3, level=3, swizzle=zzww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s3.zzww, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol4, level=4, swizzle=zwxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			texld oC0, r0, s4.zwxx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol4, level=4, swizzle=zwxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s4.zwxy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol4, level=4, swizzle=zwxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s4.zwxz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol4, level=4, swizzle=zwxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s4.zwxw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol4, level=4, swizzle=zwyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[4] = Point;
        MaxMipLevel[4] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s4
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s4.zwyx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol5, level=5, swizzle=zwyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			texld oC0, r0, s5.zwyy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol5, level=5, swizzle=zwyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s5.zwyz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol5, level=5, swizzle=zwyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s5.zwyw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol5, level=5, swizzle=zwzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s5.zwzx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol5, level=5, swizzle=zwzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s5
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s5.zwzy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol6, level=0, swizzle=zwzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			texld oC0, r0, s6.zwzz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol6, level=0, swizzle=zwzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s6.zwzw
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol6, level=0, swizzle=zwwx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s6.zwwx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol6, level=0, swizzle=zwwy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s6.zwwy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol6, level=0, swizzle=zwwz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s6
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s6.zwwz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol7, level=1, swizzle=zwww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			texld oC0, r0, s7.zwww
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol7, level=1, swizzle=wxxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s7.wxxx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol7, level=1, swizzle=wxxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s7.wxxy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol7, level=1, swizzle=wxxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s7.wxxz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol7, level=1, swizzle=wxxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s7
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s7.wxxw, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol8, level=2, swizzle=wxyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			texld oC0, r0, s8.wxyx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol8, level=2, swizzle=wxyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s8.wxyy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol8, level=2, swizzle=wxyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s8.wxyz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol8, level=2, swizzle=wxyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s8.wxyw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol8, level=2, swizzle=wxzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s8
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s8.wxzx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol9, level=3, swizzle=wxzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			texld oC0, r0, s9.wxzy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol9, level=3, swizzle=wxzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s9.wxzz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol9, level=3, swizzle=wxzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s9.wxzw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol9, level=3, swizzle=wxwx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s9.wxwx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol9, level=3, swizzle=wxwy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s9.wxwy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol10, level=4, swizzle=wxwz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			texld oC0, r0, s10.wxwz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol10, level=4, swizzle=wxww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s10.wxww
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol10, level=4, swizzle=wyxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s10.wyxx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol10, level=4, swizzle=wyxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s10.wyxy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol10, level=4, swizzle=wyxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s10
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s10.wyxz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol11, level=5, swizzle=wyxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			texld oC0, r0, s11.wyxw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol11, level=5, swizzle=wyyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s11.wyyx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol11, level=5, swizzle=wyyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s11.wyyy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol11, level=5, swizzle=wyyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s11.wyyz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol11, level=5, swizzle=wyyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s11
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s11.wyyw, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol12, level=0, swizzle=wyzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			texld oC0, r0, s12.wyzx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol12, level=0, swizzle=wyzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s12.wyzy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol12, level=0, swizzle=wyzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s12.wyzz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol12, level=0, swizzle=wyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s12.wyzw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol12, level=0, swizzle=wywx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s12
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s12.wywx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol13, level=1, swizzle=wywy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			texld oC0, r0, s13.wywy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol13, level=1, swizzle=wywz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s13.wywz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol13, level=1, swizzle=wyww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s13.wyww
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol13, level=1, swizzle=wzxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s13.wzxx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol13, level=1, swizzle=wzxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s13
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s13.wzxy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol14, level=2, swizzle=wzxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			texld oC0, r0, s14.wzxz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol14, level=2, swizzle=wzxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s14.wzxw
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol14, level=2, swizzle=wzyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 2.0, 2.0, 2.0, 2.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s14.wzyx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol14, level=2, swizzle=wzyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s14.wzyy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol14, level=2, swizzle=wzyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s14.wzyz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol15, level=3, swizzle=wzyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			texld oC0, r0, s15.wzyw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol15, level=3, swizzle=wzzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s15.wzzx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol15, level=3, swizzle=wzzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 3.0, 3.0, 3.0, 3.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s15.wzzy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol15, level=3, swizzle=wzzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s15.wzzz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol15, level=3, swizzle=wzzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s15
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s15.wzzw, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol0, level=4, swizzle=wzwx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			texld oC0, r0, s0.wzwx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol0, level=4, swizzle=wzwy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s0.wzwy
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol0, level=4, swizzle=wzwz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 4.0, 4.0, 4.0, 4.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s0.wzwz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol0, level=4, swizzle=wzww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s0.wzww
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol0, level=4, swizzle=wwxx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[0] = Point;
        MaxMipLevel[0] = 4;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s0
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s0.wwxx, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol1, level=5, swizzle=wwxy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			texld oC0, r0, s1.wwxy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol1, level=5, swizzle=wwxz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s1.wwxz
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol1, level=5, swizzle=wwxw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 5.0, 5.0, 5.0, 5.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s1.wwxw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol1, level=5, swizzle=wwyx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s1.wwyx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol1, level=5, swizzle=wwyy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s1.wwyy, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol2, level=0, swizzle=wwyz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			texld oC0, r0, s2.wwyz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol2, level=0, swizzle=wwyw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s2.wwyw
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol2, level=0, swizzle=wwzx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s2.wwzx
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol2, level=0, swizzle=wwzy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s2.wwzy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol2, level=0, swizzle=wwzz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s2.wwzz, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texld : Vol3, level=1, swizzle=wwzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			texld oC0, r0, s3.wwzw
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldl : Vol3, level=1, swizzle=wwwx";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl oC0, r0, s3.wwwx
			
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "texldb : Vol3, level=1, swizzle=wwwy";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldb oC0, r0, s3.wwwy
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldp : Vol3, level=1, swizzle=wwwz";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 1.0, 1.0, 1.0, 1.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz				
			mov r0.w, c0.x
			texldp oC0, r0, s3.wwwz
			
        };
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "texldd : Vol3, level=1, swizzle=wwww";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		CullMode = None;               
        MipFilter[3] = Point;
        MaxMipLevel[3] = 1;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def  c0, 0.0, 0.0, 0.0, 0.0
			dcl_texcoord0 v0.xyz
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldd oC0, r0, s3.wwww, c0, c0
			
        };
		VertexShader = <VolTLVS>;
   }
}
