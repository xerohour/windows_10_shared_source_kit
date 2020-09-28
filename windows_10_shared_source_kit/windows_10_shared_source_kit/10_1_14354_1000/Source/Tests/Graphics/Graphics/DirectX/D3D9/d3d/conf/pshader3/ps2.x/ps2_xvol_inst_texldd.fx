#include "include.fx"

Technique 
< 
	String	Name = "ps_2_x texldd : t0, VolTex1, dsx(r0)=(0, 0, 0.6, 1), dsy(r1)=(0.3, 0, 0, 1) -> level = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c2, 0.3, 0.0, 0.0, 1.0
            def  c3, 0.0, 0.0, 0.6, 1.0
            dcl_volume s1
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			mov r1, c2
			texldd r0, t0, s1, r0, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, VolTex1(SliceY), dsx(r0)=(0.3, 0, 0, 1), dsy(r1)=(0, 0.3, 0, 1) -> level = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c2, 0.3, 0.0, 0.0, 1.0
            dcl_volume s1
			
			mov r0, c2
			mov r1, c2
			mov r1.x, c2.y
			mov r1.y, c2.x
			texldd r0, t0, s1, r0, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, VolTex1, dsx=t0, dsy(r1)=(0.7778, 0.7778, 0, 1) -> level = NA(5)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			dcl t1
			def  c2, 0.7778, 0.7778, 0.0, 1.0	// 0.7778 = 1.1 / sqrt(2)
            dcl_volume s1
			
			mov r1, c2
			mov r0, t0
			texldd r0, r0, s1, t1, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, VolTex1(SliceY), dsx=t0, dsy(r1)=(0.7778, 0, 1.5556, 0) -> level = NA(5)";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			dcl t1
			def  c2, 0.7778, 0.0, 1.5556, 0.0	// 0.7778 = 1.1 / sqrt(2)
            dcl_volume s1
			
			mov r1, c2
			mov r0, t0
			texldd r0, r0, s1, t1, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "ps_2_x texldd : t0, VolTex1, dsx(r0)=(0.13, 0, 0, 1), dsy(c2)=(0, 0.13, 0, 1) -> level = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c2, 0.0, 0.13, 0.0, 0.0
            dcl_volume s1
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			texldd r2, t0, s1, r0, c2
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, VolTex1(SliceX), dsx(r0)=(0, 0, 0.26, 0), dsy(c2)=(0, 0.13, 0, 1) -> level = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c2, 0.0, 0.13, 0.0, 1.0
            def  c3, 0.0, 0.0, 0.26, 0.0
            dcl_volume s1
			
			mov r0, c3
			texldd r2, t0, s1, r0, c2
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "ps_2_x texldd_pp : r0, VolTex1, dsx=(1.1, 0, 0, 1), dsy=(1.1, 0, 0, 1) -> level = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c2, 1.1, 0.0, 0.0, 1.0
            dcl_volume s1
			
			mov r0, t0
			texldd_pp r0, r0, s1, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd_pp : r0, VolTex1(SliceY), dsx=(0, 0, 2.2, 1), dsy=(0, 0, 2.2, 1) -> level = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c2, 0.0, 0.0, 2.2, 1.0
            dcl_volume s1
			
			mov r0, t0
			texldd_pp r0, r0, s1, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=0/w+1/4w, dsy=1/h+1/4h; repeat=0.5 => level=0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0.f, 1.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/w+1/4w, dsy=0/h+1/4h; repeat=0.5 => lev=0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 1.f, 0.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=0/w+1/4w, dsy=1/h+1/4h; repeat=0.5 => lev=0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0.f, 1.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=2/w+1/4w, dsy=1/h+1/4h; repeat=0.5 => level=1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 1.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/w+1/4w, dsy=2/h+1/4h; repeat=0.5 => lev=1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 1.f, 2.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=2/w+1/4w, dsy=1/h+1/4h; repeat=0.5 => lev=1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 1.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=2/w+1/4w, dsy=4/h+1/4h; repeat=0.5 => level=2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 4.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=4/w+1/4w, dsy=2/h+1/4h; repeat=0.5 => lev=2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 4.f, 2.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=2/w+1/4w, dsy=4/h+1/4h; repeat=0.5 => lev=2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 4.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=8/w+1/4w, dsy=4/h+1/4h; repeat=0.5 => level=3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 4.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=4/w+1/4w, dsy=8/h+1/4h; repeat=0.5 => lev=3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 4.f, 8.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=8/w+1/4w, dsy=4/h+1/4h; repeat=0.5 => lev=3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 4.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=8/w+1/4w, dsy=16/h+1/4h; repeat=0.5 => level=4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 16.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=16/w+1/4w, dsy=8/h+1/4h; repeat=0.5 => lev=4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 16.f, 8.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=8/w+1/4w, dsy=16/h+1/4h; repeat=0.5 => lev=4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 16.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=32/w+1/4w, dsy=16/h+1/4h; repeat=0.5 => level=5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 32.f, 16.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=16/w+1/4w, dsy=32/h+1/4h; repeat=0.5 => lev=5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 16.f, 32.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=32/w+1/4w, dsy=16/h+1/4h; repeat=0.5 => lev=5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 32.f, 16.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 2;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 2;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 2;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 3;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 3;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 3;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 4;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 4;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 4;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 2;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 2;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 2;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 3;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 3;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 3;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 4;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 4;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 4;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[1] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
		MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[1] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol1(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -0";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol1(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[1] = -5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s1
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s1, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r1, VolTex6, dsx(c2)=(0.09192, 0.09192, 0, 0), dsy(c2)=(0.09192, 0.09192, 0, 0) -> level = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
			def  c2, 0.09192, 0.09192, 0.0, 0.0		// 0.09192 = 0.13 / sqrt(2)
            dcl_volume s6
			
			mov r1, t1
			texldd r0, r1, s6, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r1, VolTex6(SliceX), dsx(c2)=(0, 0.09192, 0.18384, 0), dsy(c2)=(0, 0.09192, 0.18384, 0) -> level = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
			def  c2, 0.0, 0.09192, 0.18384, 0.0		// 0.09192 = 0.13 / sqrt(2)
            dcl_volume s6
			
			mov r1, t1
			texldd r0, r1, s6, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t1, VolTex6, dsx(r0)=(0, 0, 0.6, 1), dsy(r1)=(0.3, 0, 0, 1) -> level = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
			def  c2, 0.3, 0.0, 0.0, 1.0
            def  c3, 0.0, 0.0, 0.6, 1.0
            dcl_volume s6
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			mov r1, c2
			texldd r0, t1, s6, r0, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t1, VolTex6(SliceY), dsx(r0)=(0.3, 0, 0, 1), dsy(r1)=(0, 0.3, 0, 1) -> level = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
			def  c2, 0.3, 0.0, 0.0, 1.0
            dcl_volume s6
			
			mov r0, c2
			mov r1, c2
			mov r1.x, c2.y
			mov r1.y, c2.x
			texldd r0, t1, s6, r0, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, VolTex6, dsx=t1, dsy(r1)=(0.7778, 0.7778, 0, 1) -> level = NA(5)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			dcl t1
			def  c2, 0.7778, 0.7778, 0.0, 1.0	// 0.7778 = 1.1 / sqrt(2)
            dcl_volume s6
			
			mov r1, c2
			mov r0, t0
			texldd r0, r0, s6, t1, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, VolTex6(SliceY), dsx=t1, dsy(r1)=(0.7778, 0, 1.5556, 0) -> level = NA(5)";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			dcl t1
			def  c2, 0.7778, 0.0, 1.5556, 0.0	// 0.7778 = 1.1 / sqrt(2)
            dcl_volume s6
			
			mov r1, c2
			mov r0, t0
			texldd r0, r0, s6, t1, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "ps_2_x texldd : r0, VolTex6, dsx(r1)=(0, 0.232535, 0.46507, 1), dsy=t1 -> level = NA(1)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			dcl t1
			def  c2, 0.0, 0.232535, 0.46507, 1.0	// 0.232535=.07/sqrt(2)
            dcl_volume s6
			
			mov r0, t0
			mov r1, c2
			texldd r2, r0, s6, r1, t1
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t1, VolTex6, dsx(r0)=(0.13, 0, 0, 1), dsy(c2)=(0, 0.13, 0, 1) -> level = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
			def  c2, 0.0, 0.13, 0.0, 0.0
            dcl_volume s6
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			texldd r2, t1, s6, r0, c2
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t1, VolTex6(SliceX), dsx(r0)=(0, 0, 0.26, 0), dsy(c2)=(0, 0.13, 0, 1) -> level = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
			def  c2, 0.0, 0.13, 0.0, 1.0
            def  c3, 0.0, 0.0, 0.26, 0.0
            dcl_volume s6
			
			mov r0, c3
			texldd r2, t1, s6, r0, c2
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "ps_2_x texldd_pp : r1, VolTex6, dsx=(1.1, 0, 0, 1), dsy=(1.1, 0, 0, 1) -> level = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
			def  c2, 1.1, 0.0, 0.0, 1.0
            dcl_volume s6
			
			mov r1, t1
			texldd_pp r0, r1, s6, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd_pp : r1, VolTex6(SliceY), dsx=(0, 0, 2.2, 1), dsy=(0, 0, 2.2, 1) -> level = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
			def  c2, 0.0, 0.0, 2.2, 1.0
            dcl_volume s6
			
			mov r1, t1
			texldd_pp r0, r1, s6, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=0/w+1/4w, dsy=1/h+1/4h; repeat=1.5 => level=0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0.f, 1.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/w+1/4w, dsy=0/h+1/4h; repeat=1.5 => lev=0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 1.f, 0.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=0/w+1/4w, dsy=1/h+1/4h; repeat=1.5 => lev=0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0.f, 1.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=2/w+1/4w, dsy=1/h+1/4h; repeat=1.5 => level=1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 1.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/w+1/4w, dsy=2/h+1/4h; repeat=1.5 => lev=1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 1.f, 2.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=2/w+1/4w, dsy=1/h+1/4h; repeat=1.5 => lev=1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 1.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=2/w+1/4w, dsy=4/h+1/4h; repeat=1.5 => level=2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 4.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=4/w+1/4w, dsy=2/h+1/4h; repeat=1.5 => lev=2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 4.f, 2.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=2/w+1/4w, dsy=4/h+1/4h; repeat=1.5 => lev=2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 4.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=8/w+1/4w, dsy=4/h+1/4h; repeat=1.5 => level=3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 4.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=4/w+1/4w, dsy=8/h+1/4h; repeat=1.5 => lev=3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 4.f, 8.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=8/w+1/4w, dsy=4/h+1/4h; repeat=1.5 => lev=3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 4.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=8/w+1/4w, dsy=16/h+1/4h; repeat=1.5 => level=4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 16.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=16/w+1/4w, dsy=8/h+1/4h; repeat=1.5 => lev=4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 16.f, 8.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=8/w+1/4w, dsy=16/h+1/4h; repeat=1.5 => lev=4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 16.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=32/w+1/4w, dsy=16/h+1/4h; repeat=1.5 => level=5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 32.f, 16.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=16/w+1/4w, dsy=32/h+1/4h; repeat=1.5 => lev=5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 16.f, 32.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=32/w+1/4w, dsy=16/h+1/4h; repeat=1.5 => lev=5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 32.f, 16.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 1;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 1;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 1;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 3;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 3;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 3;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 5;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 5;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 5;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 1;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 1;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 1;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 2;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 3;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 3;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 3;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 4;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 5;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 5;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[6] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 5;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[6] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol6(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol6(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[6] = -5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[6] = <TexVol_0>;
 		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s6
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s6, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r2, VolTex11, dsx(c2)=(0.09192, 0.09192, 0, 0), dsy(c2)=(0.09192, 0.09192, 0, 0) -> level = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
			def  c2, 0.09192, 0.09192, 0.0, 0.0		// 0.09192 = 0.13 / sqrt(2)
            dcl_volume s11
			
			mov r2, t2
			texldd r0, r2, s11, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r2, VolTex11(SliceX), dsx(c2)=(0, 0.09192, 0.18384, 0), dsy(c2)=(0, 0.09192, 0.18384, 0) -> level = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
			def  c2, 0.0, 0.09192, 0.18384, 0.0		// 0.09192 = 0.13 / sqrt(2)
            dcl_volume s11
			
			mov r2, t2
			texldd r0, r2, s11, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t2, VolTex11, dsx(r0)=(0, 0, 0.6, 1), dsy(r1)=(0.3, 0, 0, 1) -> level = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
			def  c2, 0.3, 0.0, 0.0, 1.0
            def  c3, 0.0, 0.0, 0.6, 1.0
            dcl_volume s11
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			mov r1, c2
			texldd r0, t2, s11, r0, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t2, VolTex11(SliceY), dsx(r0)=(0.3, 0, 0, 1), dsy(r1)=(0, 0.3, 0, 1) -> level = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
			def  c2, 0.3, 0.0, 0.0, 1.0
            dcl_volume s11
			
			mov r0, c2
			mov r1, c2
			mov r1.x, c2.y
			mov r1.y, c2.x
			texldd r0, t2, s11, r0, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, VolTex11, dsx=t2, dsy(r1)=(0.7778, 0.7778, 0, 1) -> level = NA(5)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			dcl t1
			def  c2, 0.7778, 0.7778, 0.0, 1.0	// 0.7778 = 1.1 / sqrt(2)
            dcl_volume s11
			
			mov r1, c2
			mov r0, t0
			texldd r0, r0, s11, t1, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, VolTex11(SliceY), dsx=t2, dsy(r1)=(0.7778, 0, 1.5556, 0) -> level = NA(5)";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			dcl t1
			def  c2, 0.7778, 0.0, 1.5556, 0.0	// 0.7778 = 1.1 / sqrt(2)
            dcl_volume s11
			
			mov r1, c2
			mov r0, t0
			texldd r0, r0, s11, t1, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "ps_2_x texldd : t2, VolTex11, dsx(r0)=(0.13, 0, 0, 1), dsy(c2)=(0, 0.13, 0, 1) -> level = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
			def  c2, 0.0, 0.13, 0.0, 0.0
            dcl_volume s11
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			texldd r2, t2, s11, r0, c2
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t2, VolTex11(SliceX), dsx(r0)=(0, 0, 0.26, 0), dsy(c2)=(0, 0.13, 0, 1) -> level = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
			def  c2, 0.0, 0.13, 0.0, 1.0
            def  c3, 0.0, 0.0, 0.26, 0.0
            dcl_volume s11
			
			mov r0, c3
			texldd r2, t2, s11, r0, c2
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "ps_2_x texldd_pp : r2, VolTex11, dsx=(1.1, 0, 0, 1), dsy=(1.1, 0, 0, 1) -> level = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
			def  c2, 1.1, 0.0, 0.0, 1.0
            dcl_volume s11
			
			mov r2, t2
			texldd_pp r0, r2, s11, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd_pp : r2, VolTex11(SliceY), dsx=(0, 0, 2.2, 1), dsy=(0, 0, 2.2, 1) -> level = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
			def  c2, 0.0, 0.0, 2.2, 1.0
            dcl_volume s11
			
			mov r2, t2
			texldd_pp r0, r2, s11, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=0/w+1/4w, dsy=1/h+1/4h; repeat=2.5 => level=0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0.f, 1.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/w+1/4w, dsy=0/h+1/4h; repeat=2.5 => lev=0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 1.f, 0.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=0/w+1/4w, dsy=1/h+1/4h; repeat=2.5 => lev=0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0.f, 1.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=2/w+1/4w, dsy=1/h+1/4h; repeat=2.5 => level=1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 1.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/w+1/4w, dsy=2/h+1/4h; repeat=2.5 => lev=1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 1.f, 2.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=2/w+1/4w, dsy=1/h+1/4h; repeat=2.5 => lev=1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 1.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=2/w+1/4w, dsy=4/h+1/4h; repeat=2.5 => level=2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 4.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=4/w+1/4w, dsy=2/h+1/4h; repeat=2.5 => lev=2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 4.f, 2.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=2/w+1/4w, dsy=4/h+1/4h; repeat=2.5 => lev=2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 2.f, 4.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=8/w+1/4w, dsy=4/h+1/4h; repeat=2.5 => level=3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 4.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=4/w+1/4w, dsy=8/h+1/4h; repeat=2.5 => lev=3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 4.f, 8.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=8/w+1/4w, dsy=4/h+1/4h; repeat=2.5 => lev=3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 4.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=8/w+1/4w, dsy=16/h+1/4h; repeat=2.5 => level=4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 16.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=16/w+1/4w, dsy=8/h+1/4h; repeat=2.5 => lev=4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 16.f, 8.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=8/w+1/4w, dsy=16/h+1/4h; repeat=2.5 => lev=4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 8.f, 16.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=32/w+1/4w, dsy=16/h+1/4h; repeat=2.5 => level=5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 32.f, 16.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=16/w+1/4w, dsy=32/h+1/4h; repeat=2.5 => lev=5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 16.f, 32.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=32/w+1/4w, dsy=16/h+1/4h; repeat=2.5 => lev=5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 32.f, 16.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 2;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 2;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 2;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 4;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 4;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 4;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x 
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 1;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 2;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 2;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 2;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 3;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 4;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 4;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 4;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[11] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 5;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[11] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : t0, TexVol11(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, t0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldd : r0, TexVol11(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[11] = -5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[11] = <TexVol_0>;
 		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s11
			
			mov r0, t0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s11, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
