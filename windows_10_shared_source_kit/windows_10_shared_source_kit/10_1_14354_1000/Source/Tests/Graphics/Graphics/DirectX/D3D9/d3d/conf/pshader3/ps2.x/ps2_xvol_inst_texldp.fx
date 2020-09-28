#include "include.fx"

Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1.800000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 1.800000, 1.800000, 1.800000, 1.800000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1.800000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 1.800000, 1.800000, 1.800000, 1.800000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1.800000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 1.800000, 1.800000, 1.800000, 1.800000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 4.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 4.000000, 4.000000, 4.000000, 4.000000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 4.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 4.000000, 4.000000, 4.000000, 4.000000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 4.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 4.000000, 4.000000, 4.000000, 4.000000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = -2.800000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, -2.800000, -2.800000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = -2.800000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, -2.800000, -2.800000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = -2.800000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, -2.800000, -2.800000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = -2.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, -2.000000, -2.000000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = -2.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, -2.000000, -2.000000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = -2.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, -2.000000, -2.000000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/1.500000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.666667, 0.666667
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/1.500000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.666667, 0.666667
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/1.500000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.666667, 0.666667
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/-3.500000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, -0.285714, -0.285714
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/-3.500000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, -0.285714, -0.285714
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/-3.500000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, -0.285714, -0.285714
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 1.000000, 1.000000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 1.000000, 1.000000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 1.000000, 1.000000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.500000, 0.500000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.500000, 0.500000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.500000, 0.500000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.250000, 0.250000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.250000, 0.250000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.250000, 0.250000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/8";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.125000, 0.125000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/8";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.125000, 0.125000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/8";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.125000, 0.125000
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/16";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.062500, 0.062500
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/16";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.062500, 0.062500
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/16";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.062500, 0.062500
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/32";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.031250, 0.031250
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/32";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.031250, 0.031250
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/32";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.031250, 0.031250
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/64";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.015625, 0.015625
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/64";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.015625, 0.015625
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/64";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.0, 0.0, 0.015625, 0.015625
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; w = 1, MaxMipLevel[5] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceX, w = 1, MaxMipLevel[5] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceY, w = 1, MaxMipLevel[5] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; w = 1, MaxMipLevel[5] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceX, w = 1, MaxMipLevel[5] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceY, w = 1, MaxMipLevel[5] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; w = 1, MaxMipLevel[5] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 2;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceX, w = 1, MaxMipLevel[5] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 2;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceY, w = 1, MaxMipLevel[5] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 2;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; w = 1, MaxMipLevel[5] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceX, w = 1, MaxMipLevel[5] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceY, w = 1, MaxMipLevel[5] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; w = 1, MaxMipLevel[5] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 4;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceX, w = 1, MaxMipLevel[5] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 4;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceY, w = 1, MaxMipLevel[5] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 4;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; w = 1, MaxMipLevel[5] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceX, w = 1, MaxMipLevel[5] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceY, w = 1, MaxMipLevel[5] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; w = 1, MaxMipLevel[5] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 6;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceX, w = 1, MaxMipLevel[5] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 6;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceY, w = 1, MaxMipLevel[5] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 6;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol5; w = 1, MaxMipLevel[5] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r0, t1
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol5; sliceX, w = 1, MaxMipLevel[5] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r0, t1
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol5; sliceY, w = 1, MaxMipLevel[5] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r0, t1
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol5; w = 1, MaxMipLevel[5] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r1, t1
			texldp r1, r1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol5; sliceX, w = 1, MaxMipLevel[5] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r1, t1
			texldp r1, r1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol5; sliceY, w = 1, MaxMipLevel[5] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r1, t1
			texldp r1, r1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol5; w = 1, MaxMipLevel[5] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 2;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r2, t1
			texldp r1, r2, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol5; sliceX, w = 1, MaxMipLevel[5] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 2;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r2, t1
			texldp r1, r2, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol5; sliceY, w = 1, MaxMipLevel[5] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 2;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r2, t1
			texldp r1, r2, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol5; w = 1, MaxMipLevel[5] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r3, t1
			texldp r1, r3, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol5; sliceX, w = 1, MaxMipLevel[5] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r3, t1
			texldp r1, r3, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol5; sliceY, w = 1, MaxMipLevel[5] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r3, t1
			texldp r1, r3, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol5; w = 1, MaxMipLevel[5] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 4;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r4, t1
			texldp r1, r4, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol5; sliceX, w = 1, MaxMipLevel[5] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 4;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r4, t1
			texldp r1, r4, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol5; sliceY, w = 1, MaxMipLevel[5] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 4;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r4, t1
			texldp r1, r4, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol5; w = 1, MaxMipLevel[5] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r5, t1
			texldp r1, r5, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol5; sliceX, w = 1, MaxMipLevel[5] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r5, t1
			texldp r1, r5, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol5; sliceY, w = 1, MaxMipLevel[5] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r5, t1
			texldp r1, r5, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol5; w = 1, MaxMipLevel[5] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 6;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r6, t1
			texldp r1, r6, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol5; sliceX, w = 1, MaxMipLevel[5] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 6;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r6, t1
			texldp r1, r6, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol5; sliceY, w = 1, MaxMipLevel[5] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 6;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r6, t1
			texldp r1, r6, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; w = 1, MipMapLODBias[5] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceX, w = 1, MipMapLODBias[5] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceY, w = 1, MipMapLODBias[5] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; w = 1, MipMapLODBias[5] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceX, w = 1, MipMapLODBias[5] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceY, w = 1, MipMapLODBias[5] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; w = 1, MipMapLODBias[5] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceX, w = 1, MipMapLODBias[5] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceY, w = 1, MipMapLODBias[5] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; w = 1, MipMapLODBias[5] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceX, w = 1, MipMapLODBias[5] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceY, w = 1, MipMapLODBias[5] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; w = 1, MipMapLODBias[5] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceX, w = 1, MipMapLODBias[5] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceY, w = 1, MipMapLODBias[5] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; w = 1, MipMapLODBias[5] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceX, w = 1, MipMapLODBias[5] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceY, w = 1, MipMapLODBias[5] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; w = 1, MipMapLODBias[5] = 6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceX, w = 1, MipMapLODBias[5] = 6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol5; sliceY, w = 1, MipMapLODBias[5] = 6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			texldp r1, t1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol5; w = 1, MipMapLODBias[5] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r0, t1
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol5; sliceX, w = 1, MipMapLODBias[5] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r0, t1
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol5; sliceY, w = 1, MipMapLODBias[5] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r0, t1
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol5; w = 1, MipMapLODBias[5] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r1, t1
			texldp r1, r1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol5; sliceX, w = 1, MipMapLODBias[5] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r1, t1
			texldp r1, r1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol5; sliceY, w = 1, MipMapLODBias[5] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r1, t1
			texldp r1, r1, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol5; w = 1, MipMapLODBias[5] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r2, t1
			texldp r1, r2, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol5; sliceX, w = 1, MipMapLODBias[5] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r2, t1
			texldp r1, r2, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol5; sliceY, w = 1, MipMapLODBias[5] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r2, t1
			texldp r1, r2, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol5; w = 1, MipMapLODBias[5] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r3, t1
			texldp r1, r3, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol5; sliceX, w = 1, MipMapLODBias[5] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r3, t1
			texldp r1, r3, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol5; sliceY, w = 1, MipMapLODBias[5] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r3, t1
			texldp r1, r3, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol5; w = 1, MipMapLODBias[5] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r4, t1
			texldp r1, r4, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol5; sliceX, w = 1, MipMapLODBias[5] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r4, t1
			texldp r1, r4, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol5; sliceY, w = 1, MipMapLODBias[5] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r4, t1
			texldp r1, r4, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol5; w = 1, MipMapLODBias[5] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r5, t1
			texldp r1, r5, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol5; sliceX, w = 1, MipMapLODBias[5] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r5, t1
			texldp r1, r5, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol5; sliceY, w = 1, MipMapLODBias[5] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r5, t1
			texldp r1, r5, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol5; w = 1, MipMapLODBias[5] = 6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r6, t1
			texldp r1, r6, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol5; sliceX, w = 1, MipMapLODBias[5] = 6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r6, t1
			texldp r1, r6, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol5; sliceY, w = 1, MipMapLODBias[5] = 6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s5
			
			mov r6, t1
			texldp r1, r6, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/32, MipMapLODBias[5] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/32, MipMapLODBias[5] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/32, MipMapLODBias[5]=-0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/32, MipMapLODBias[5] = -1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/32, MipMapLODBias[5] = -1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/32, MipMapLODBias[5]=-1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/32, MipMapLODBias[5] = -2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/32, MipMapLODBias[5] = -2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/32, MipMapLODBias[5]=-2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/32, MipMapLODBias[5] = -3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/32, MipMapLODBias[5] = -3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/32, MipMapLODBias[5]=-3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/32, MipMapLODBias[5] = -4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/32, MipMapLODBias[5] = -4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/32, MipMapLODBias[5]=-4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/32, MipMapLODBias[5] = -5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/32, MipMapLODBias[5] = -5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/32, MipMapLODBias[5]=-5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, w = 1/32, MipMapLODBias[5] = -6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceX, w = 1/32, MipMapLODBias[5] = -6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol5, sliceY, w = 1/32, MipMapLODBias[5]=-6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s5
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s5
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol7";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r1, t1
			texldp r1, r1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol7, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r1, t1
			texldp r1, r1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol7, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r1, t1
			texldp r1, r1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7, writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1.xyzw, t1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7, sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1.xyzw, t1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7, sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1.xyzw, t1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol7, writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r1, t1
			texldp r1.xyzw, r1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol7, sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r1, t1
			texldp r1.xyzw, r1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol7, sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r1, t1
			texldp r1.xyzw, r1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : t1, TexVol7";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp_pp r1, t1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : t1, TexVol7, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp_pp r1, t1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : t1, TexVol7, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp_pp r1, t1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : r1, TexVol7";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r1, t1
			texldp_pp r1, r1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : r1, TexVol7, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r1, t1
			texldp_pp r1, r1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : r1, TexVol7, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r1, t1
			texldp_pp r1, r1, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 3.600000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 3.600000, 3.600000, 3.600000, 3.600000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 3.600000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 3.600000, 3.600000, 3.600000, 3.600000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 3.600000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 3.600000, 3.600000, 3.600000, 3.600000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 8.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 8.000000, 8.000000, 8.000000, 8.000000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 8.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 8.000000, 8.000000, 8.000000, 8.000000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 8.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 8.000000, 8.000000, 8.000000, 8.000000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = -5.600000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -5.600000, -5.600000, -5.600000, -5.600000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = -5.600000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -5.600000, -5.600000, -5.600000, -5.600000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = -5.600000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -5.600000, -5.600000, -5.600000, -5.600000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = -4.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -4.000000, -4.000000, -4.000000, -4.000000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = -4.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -4.000000, -4.000000, -4.000000, -4.000000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = -4.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -4.000000, -4.000000, -4.000000, -4.000000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/3.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.333333, 0.333333, 0.333333, 0.333333
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/3.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.333333, 0.333333, 0.333333, 0.333333
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/3.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.333333, 0.333333, 0.333333, 0.333333
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/-7.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -0.142857, -0.142857, -0.142857, -0.142857
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/-7.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -0.142857, -0.142857, -0.142857, -0.142857
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/-7.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -0.142857, -0.142857, -0.142857, -0.142857
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/8";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/8";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/8";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/16";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/16";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/16";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/32";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/32";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/32";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/64";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/64";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/64";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; w = 1, MaxMipLevel[7] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceX, w = 1, MaxMipLevel[7] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceY, w = 1, MaxMipLevel[7] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; w = 1, MaxMipLevel[7] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceX, w = 1, MaxMipLevel[7] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceY, w = 1, MaxMipLevel[7] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; w = 1, MaxMipLevel[7] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceX, w = 1, MaxMipLevel[7] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceY, w = 1, MaxMipLevel[7] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; w = 1, MaxMipLevel[7] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceX, w = 1, MaxMipLevel[7] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceY, w = 1, MaxMipLevel[7] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; w = 1, MaxMipLevel[7] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceX, w = 1, MaxMipLevel[7] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceY, w = 1, MaxMipLevel[7] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; w = 1, MaxMipLevel[7] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceX, w = 1, MaxMipLevel[7] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceY, w = 1, MaxMipLevel[7] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; w = 1, MaxMipLevel[7] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceX, w = 1, MaxMipLevel[7] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceY, w = 1, MaxMipLevel[7] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol7; w = 1, MaxMipLevel[7] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r0, t1
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol7; sliceX, w = 1, MaxMipLevel[7] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r0, t1
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol7; sliceY, w = 1, MaxMipLevel[7] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r0, t1
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol7; w = 1, MaxMipLevel[7] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r1, t1
			texldp r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol7; sliceX, w = 1, MaxMipLevel[7] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r1, t1
			texldp r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol7; sliceY, w = 1, MaxMipLevel[7] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r1, t1
			texldp r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol7; w = 1, MaxMipLevel[7] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r2, t1
			texldp r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol7; sliceX, w = 1, MaxMipLevel[7] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r2, t1
			texldp r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol7; sliceY, w = 1, MaxMipLevel[7] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r2, t1
			texldp r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol7; w = 1, MaxMipLevel[7] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r3, t1
			texldp r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol7; sliceX, w = 1, MaxMipLevel[7] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r3, t1
			texldp r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol7; sliceY, w = 1, MaxMipLevel[7] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r3, t1
			texldp r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol7; w = 1, MaxMipLevel[7] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r4, t1
			texldp r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol7; sliceX, w = 1, MaxMipLevel[7] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r4, t1
			texldp r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol7; sliceY, w = 1, MaxMipLevel[7] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r4, t1
			texldp r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol7; w = 1, MaxMipLevel[7] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r5, t1
			texldp r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol7; sliceX, w = 1, MaxMipLevel[7] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r5, t1
			texldp r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol7; sliceY, w = 1, MaxMipLevel[7] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r5, t1
			texldp r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol7; w = 1, MaxMipLevel[7] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r6, t1
			texldp r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol7; sliceX, w = 1, MaxMipLevel[7] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r6, t1
			texldp r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol7; sliceY, w = 1, MaxMipLevel[7] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r6, t1
			texldp r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; w = 1, MipMapLODBias[7] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceX, w = 1, MipMapLODBias[7] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceY, w = 1, MipMapLODBias[7] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; w = 1, MipMapLODBias[7] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceX, w = 1, MipMapLODBias[7] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceY, w = 1, MipMapLODBias[7] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; w = 1, MipMapLODBias[7] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceX, w = 1, MipMapLODBias[7] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceY, w = 1, MipMapLODBias[7] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; w = 1, MipMapLODBias[7] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceX, w = 1, MipMapLODBias[7] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceY, w = 1, MipMapLODBias[7] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; w = 1, MipMapLODBias[7] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceX, w = 1, MipMapLODBias[7] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceY, w = 1, MipMapLODBias[7] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; w = 1, MipMapLODBias[7] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceX, w = 1, MipMapLODBias[7] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceY, w = 1, MipMapLODBias[7] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; w = 1, MipMapLODBias[7] = 6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceX, w = 1, MipMapLODBias[7] = 6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol7; sliceY, w = 1, MipMapLODBias[7] = 6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			texldp r1, t1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol7; w = 1, MipMapLODBias[7] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r0, t1
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol7; sliceX, w = 1, MipMapLODBias[7] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r0, t1
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol7; sliceY, w = 1, MipMapLODBias[7] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r0, t1
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol7; w = 1, MipMapLODBias[7] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r1, t1
			texldp r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol7; sliceX, w = 1, MipMapLODBias[7] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r1, t1
			texldp r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol7; sliceY, w = 1, MipMapLODBias[7] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r1, t1
			texldp r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol7; w = 1, MipMapLODBias[7] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r2, t1
			texldp r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol7; sliceX, w = 1, MipMapLODBias[7] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r2, t1
			texldp r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol7; sliceY, w = 1, MipMapLODBias[7] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r2, t1
			texldp r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol7; w = 1, MipMapLODBias[7] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r3, t1
			texldp r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol7; sliceX, w = 1, MipMapLODBias[7] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r3, t1
			texldp r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol7; sliceY, w = 1, MipMapLODBias[7] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r3, t1
			texldp r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol7; w = 1, MipMapLODBias[7] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r4, t1
			texldp r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol7; sliceX, w = 1, MipMapLODBias[7] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r4, t1
			texldp r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol7; sliceY, w = 1, MipMapLODBias[7] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r4, t1
			texldp r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol7; w = 1, MipMapLODBias[7] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r5, t1
			texldp r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol7; sliceX, w = 1, MipMapLODBias[7] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r5, t1
			texldp r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol7; sliceY, w = 1, MipMapLODBias[7] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r5, t1
			texldp r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol7; w = 1, MipMapLODBias[7] = 6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r6, t1
			texldp r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol7; sliceX, w = 1, MipMapLODBias[7] = 6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r6, t1
			texldp r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol7; sliceY, w = 1, MipMapLODBias[7] = 6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s7
			
			mov r6, t1
			texldp r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/32, MipMapLODBias[7] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/32, MipMapLODBias[7] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/32, MipMapLODBias[7]=-0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/32, MipMapLODBias[7] = -1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/32, MipMapLODBias[7] = -1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/32, MipMapLODBias[7]=-1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/32, MipMapLODBias[7] = -2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/32, MipMapLODBias[7] = -2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/32, MipMapLODBias[7]=-2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/32, MipMapLODBias[7] = -3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/32, MipMapLODBias[7] = -3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/32, MipMapLODBias[7]=-3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/32, MipMapLODBias[7] = -4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/32, MipMapLODBias[7] = -4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/32, MipMapLODBias[7]=-4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/32, MipMapLODBias[7] = -5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/32, MipMapLODBias[7] = -5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/32, MipMapLODBias[7]=-5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, w = 1/32, MipMapLODBias[7] = -6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceX, w = 1/32, MipMapLODBias[7] = -6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol7, sliceY, w = 1/32, MipMapLODBias[7]=-6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s7
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t2, TexVol12";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			texldp r1, t2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t2, TexVol12, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			texldp r1, t2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t2, TexVol12, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			texldp r1, t2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol12";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			mov r2, t2
			texldp r1, r2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol12, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			mov r2, t2
			texldp r1, r2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol12, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			mov r2, t2
			texldp r1, r2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t2, TexVol12, writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			texldp r1.xyzw, t2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t2, TexVol12, sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			texldp r1.xyzw, t2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t2, TexVol12, sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			texldp r1.xyzw, t2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol12, writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			mov r2, t2
			texldp r1.xyzw, r2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol12, sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			mov r2, t2
			texldp r1.xyzw, r2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol12, sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			mov r2, t2
			texldp r1.xyzw, r2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : t2, TexVol12";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			texldp_pp r1, t2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : t2, TexVol12, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			texldp_pp r1, t2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : t2, TexVol12, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			texldp_pp r1, t2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : r2, TexVol12";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			mov r2, t2
			texldp_pp r1, r2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : r2, TexVol12, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			mov r2, t2
			texldp_pp r1, r2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : r2, TexVol12, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t2
            dcl_volume s12
			
			mov r2, t2
			texldp_pp r1, r2, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 5.400000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 5.400000, 5.400000, 5.400000, 5.400000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 5.400000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 5.400000, 5.400000, 5.400000, 5.400000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 5.400000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 5.400000, 5.400000, 5.400000, 5.400000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 12.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 12.000000, 12.000000, 12.000000, 12.000000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 12.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 12.000000, 12.000000, 12.000000, 12.000000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 12.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 12.000000, 12.000000, 12.000000, 12.000000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = -8.400000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -8.400000, -8.400000, -8.400000, -8.400000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = -8.400000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -8.400000, -8.400000, -8.400000, -8.400000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = -8.400000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -8.400000, -8.400000, -8.400000, -8.400000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = -6.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -6.000000, -6.000000, -6.000000, -6.000000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = -6.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -6.000000, -6.000000, -6.000000, -6.000000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = -6.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -6.000000, -6.000000, -6.000000, -6.000000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/4.500000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.222222, 0.222222, 0.222222, 0.222222
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/4.500000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.222222, 0.222222, 0.222222, 0.222222
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/4.500000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.222222, 0.222222, 0.222222, 0.222222
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/-10.500000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -0.095238, -0.095238, -0.095238, -0.095238
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/-10.500000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -0.095238, -0.095238, -0.095238, -0.095238
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/-10.500000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, -0.095238, -0.095238, -0.095238, -0.095238
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/8";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/8";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/8";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/16";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/16";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/16";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/32";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/32";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/32";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/64";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/64";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/64";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; w = 1, MaxMipLevel[12] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceX, w = 1, MaxMipLevel[12] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceY, w = 1, MaxMipLevel[12] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; w = 1, MaxMipLevel[12] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceX, w = 1, MaxMipLevel[12] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceY, w = 1, MaxMipLevel[12] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; w = 1, MaxMipLevel[12] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceX, w = 1, MaxMipLevel[12] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceY, w = 1, MaxMipLevel[12] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; w = 1, MaxMipLevel[12] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceX, w = 1, MaxMipLevel[12] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceY, w = 1, MaxMipLevel[12] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; w = 1, MaxMipLevel[12] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceX, w = 1, MaxMipLevel[12] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceY, w = 1, MaxMipLevel[12] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; w = 1, MaxMipLevel[12] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceX, w = 1, MaxMipLevel[12] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceY, w = 1, MaxMipLevel[12] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; w = 1, MaxMipLevel[12] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceX, w = 1, MaxMipLevel[12] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceY, w = 1, MaxMipLevel[12] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol12; w = 1, MaxMipLevel[12] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r0, t1
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol12; sliceX, w = 1, MaxMipLevel[12] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r0, t1
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol12; sliceY, w = 1, MaxMipLevel[12] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r0, t1
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol12; w = 1, MaxMipLevel[12] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r1, t1
			texldp r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol12; sliceX, w = 1, MaxMipLevel[12] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r1, t1
			texldp r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol12; sliceY, w = 1, MaxMipLevel[12] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r1, t1
			texldp r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol12; w = 1, MaxMipLevel[12] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r2, t1
			texldp r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol12; sliceX, w = 1, MaxMipLevel[12] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r2, t1
			texldp r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol12; sliceY, w = 1, MaxMipLevel[12] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r2, t1
			texldp r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol12; w = 1, MaxMipLevel[12] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r3, t1
			texldp r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol12; sliceX, w = 1, MaxMipLevel[12] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r3, t1
			texldp r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol12; sliceY, w = 1, MaxMipLevel[12] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r3, t1
			texldp r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol12; w = 1, MaxMipLevel[12] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r4, t1
			texldp r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol12; sliceX, w = 1, MaxMipLevel[12] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r4, t1
			texldp r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol12; sliceY, w = 1, MaxMipLevel[12] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r4, t1
			texldp r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol12; w = 1, MaxMipLevel[12] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r5, t1
			texldp r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol12; sliceX, w = 1, MaxMipLevel[12] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r5, t1
			texldp r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol12; sliceY, w = 1, MaxMipLevel[12] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r5, t1
			texldp r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol12; w = 1, MaxMipLevel[12] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r6, t1
			texldp r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol12; sliceX, w = 1, MaxMipLevel[12] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r6, t1
			texldp r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol12; sliceY, w = 1, MaxMipLevel[12] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r6, t1
			texldp r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; w = 1, MipMapLODBias[12] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceX, w = 1, MipMapLODBias[12] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceY, w = 1, MipMapLODBias[12] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; w = 1, MipMapLODBias[12] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceX, w = 1, MipMapLODBias[12] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceY, w = 1, MipMapLODBias[12] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; w = 1, MipMapLODBias[12] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceX, w = 1, MipMapLODBias[12] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceY, w = 1, MipMapLODBias[12] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; w = 1, MipMapLODBias[12] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceX, w = 1, MipMapLODBias[12] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceY, w = 1, MipMapLODBias[12] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; w = 1, MipMapLODBias[12] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceX, w = 1, MipMapLODBias[12] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceY, w = 1, MipMapLODBias[12] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; w = 1, MipMapLODBias[12] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceX, w = 1, MipMapLODBias[12] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceY, w = 1, MipMapLODBias[12] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; w = 1, MipMapLODBias[12] = 6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceX, w = 1, MipMapLODBias[12] = 6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, TexVol12; sliceY, w = 1, MipMapLODBias[12] = 6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			texldp r1, t1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol12; w = 1, MipMapLODBias[12] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r0, t1
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol12; sliceX, w = 1, MipMapLODBias[12] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r0, t1
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, TexVol12; sliceY, w = 1, MipMapLODBias[12] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r0, t1
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol12; w = 1, MipMapLODBias[12] = 1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r1, t1
			texldp r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol12; sliceX, w = 1, MipMapLODBias[12] = 1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r1, t1
			texldp r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, TexVol12; sliceY, w = 1, MipMapLODBias[12] = 1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r1, t1
			texldp r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol12; w = 1, MipMapLODBias[12] = 2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r2, t1
			texldp r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol12; sliceX, w = 1, MipMapLODBias[12] = 2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r2, t1
			texldp r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, TexVol12; sliceY, w = 1, MipMapLODBias[12] = 2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r2, t1
			texldp r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol12; w = 1, MipMapLODBias[12] = 3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r3, t1
			texldp r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol12; sliceX, w = 1, MipMapLODBias[12] = 3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r3, t1
			texldp r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, TexVol12; sliceY, w = 1, MipMapLODBias[12] = 3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r3, t1
			texldp r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol12; w = 1, MipMapLODBias[12] = 4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r4, t1
			texldp r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol12; sliceX, w = 1, MipMapLODBias[12] = 4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r4, t1
			texldp r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, TexVol12; sliceY, w = 1, MipMapLODBias[12] = 4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r4, t1
			texldp r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol12; w = 1, MipMapLODBias[12] = 5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r5, t1
			texldp r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol12; sliceX, w = 1, MipMapLODBias[12] = 5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r5, t1
			texldp r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, TexVol12; sliceY, w = 1, MipMapLODBias[12] = 5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r5, t1
			texldp r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol12; w = 1, MipMapLODBias[12] = 6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r6, t1
			texldp r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol12; sliceX, w = 1, MipMapLODBias[12] = 6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r6, t1
			texldp r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, TexVol12; sliceY, w = 1, MipMapLODBias[12] = 6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t1
            dcl_volume s12
			
			mov r6, t1
			texldp r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/32, MipMapLODBias[12] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/32, MipMapLODBias[12] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/32, MipMapLODBias[12]=-0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/32, MipMapLODBias[12] = -1";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/32, MipMapLODBias[12] = -1";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/32, MipMapLODBias[12]=-1";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/32, MipMapLODBias[12] = -2";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/32, MipMapLODBias[12] = -2";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/32, MipMapLODBias[12]=-2";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/32, MipMapLODBias[12] = -3";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/32, MipMapLODBias[12] = -3";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/32, MipMapLODBias[12]=-3";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/32, MipMapLODBias[12] = -4";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/32, MipMapLODBias[12] = -4";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/32, MipMapLODBias[12]=-4";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/32, MipMapLODBias[12] = -5";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/32, MipMapLODBias[12] = -5";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/32, MipMapLODBias[12]=-5";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, w = 1/32, MipMapLODBias[12] = -6";
	String	Shape = "VolTLZ";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			mul r0.z, r0, c0
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceX, w = 1/32, MipMapLODBias[12] = -6";
	String	Shape = "VolTLX";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexVol12, sliceY, w = 1/32, MipMapLODBias[12]=-6";
	String	Shape = "VolTLY";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s12
			
			mov r0.x, t0.x
			mov r0.y, t0.y
			mov r0.z, t0.z
			mov r0.w, c0.w
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
