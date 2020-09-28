#include "include.fx"

Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1.800000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.800000, 1.800000, 1.800000, 1.800000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1.800000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.800000, 1.800000, 1.800000, 1.800000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1.800000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.800000, 1.800000, 1.800000, 1.800000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 4.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.000000, 4.000000, 4.000000, 4.000000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 4.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.000000, 4.000000, 4.000000, 4.000000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 4.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.000000, 4.000000, 4.000000, 4.000000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = -2.800000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -2.800000, -2.800000, -2.800000, -2.800000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = -2.800000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -2.800000, -2.800000, -2.800000, -2.800000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = -2.800000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -2.800000, -2.800000, -2.800000, -2.800000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = -2.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -2.000000, -2.000000, -2.000000, -2.000000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = -2.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -2.000000, -2.000000, -2.000000, -2.000000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = -2.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -2.000000, -2.000000, -2.000000, -2.000000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/1.500000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.666667, 0.666667, 0.666667, 0.666667
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/1.500000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.666667, 0.666667, 0.666667, 0.666667
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/1.500000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.666667, 0.666667, 0.666667, 0.666667
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/-3.500000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.285714, -0.285714, -0.285714, -0.285714
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/-3.500000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.285714, -0.285714, -0.285714, -0.285714
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/-3.500000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.285714, -0.285714, -0.285714, -0.285714
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/8";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/8";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/8";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/16";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/16";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/16";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/32";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/32";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/32";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/64";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/64";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/64";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; w = 1, MaxMipLevel[1] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceX, w = 1, MaxMipLevel[1] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceY, w = 1, MaxMipLevel[1] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; w = 1, MaxMipLevel[1] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceX, w = 1, MaxMipLevel[1] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceY, w = 1, MaxMipLevel[1] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; w = 1, MaxMipLevel[1] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceX, w = 1, MaxMipLevel[1] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceY, w = 1, MaxMipLevel[1] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; w = 1, MaxMipLevel[1] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceX, w = 1, MaxMipLevel[1] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceY, w = 1, MaxMipLevel[1] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; w = 1, MaxMipLevel[1] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceX, w = 1, MaxMipLevel[1] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceY, w = 1, MaxMipLevel[1] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; w = 1, MaxMipLevel[1] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceX, w = 1, MaxMipLevel[1] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceY, w = 1, MaxMipLevel[1] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; w = 1, MaxMipLevel[1] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 6;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceX, w = 1, MaxMipLevel[1] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 6;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceY, w = 1, MaxMipLevel[1] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 6;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol1; w = 1, MaxMipLevel[1] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r0, v1
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol1; sliceX, w = 1, MaxMipLevel[1] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r0, v1
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol1; sliceY, w = 1, MaxMipLevel[1] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r0, v1
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol1; w = 1, MaxMipLevel[1] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r1, v1
			texldp r1, r1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol1; sliceX, w = 1, MaxMipLevel[1] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r1, v1
			texldp r1, r1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol1; sliceY, w = 1, MaxMipLevel[1] = 1";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r1, v1
			texldp r1, r1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol1; w = 1, MaxMipLevel[1] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r2, v1
			texldp r1, r2, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol1; sliceX, w = 1, MaxMipLevel[1] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r2, v1
			texldp r1, r2, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol1; sliceY, w = 1, MaxMipLevel[1] = 2";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r2, v1
			texldp r1, r2, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol1; w = 1, MaxMipLevel[1] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r3, v1
			texldp r1, r3, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol1; sliceX, w = 1, MaxMipLevel[1] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r3, v1
			texldp r1, r3, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol1; sliceY, w = 1, MaxMipLevel[1] = 3";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r3, v1
			texldp r1, r3, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol1; w = 1, MaxMipLevel[1] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r4, v1
			texldp r1, r4, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol1; sliceX, w = 1, MaxMipLevel[1] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r4, v1
			texldp r1, r4, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol1; sliceY, w = 1, MaxMipLevel[1] = 4";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r4, v1
			texldp r1, r4, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol1; w = 1, MaxMipLevel[1] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r5, v1
			texldp r1, r5, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol1; sliceX, w = 1, MaxMipLevel[1] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r5, v1
			texldp r1, r5, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol1; sliceY, w = 1, MaxMipLevel[1] = 5";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r5, v1
			texldp r1, r5, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol1; w = 1, MaxMipLevel[1] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 6;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r6, v1
			texldp r1, r6, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol1; sliceX, w = 1, MaxMipLevel[1] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 6;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r6, v1
			texldp r1, r6, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol1; sliceY, w = 1, MaxMipLevel[1] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 6;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r6, v1
			texldp r1, r6, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; w = 1, MipMapLODBias[1] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceX, w = 1, MipMapLODBias[1] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceY, w = 1, MipMapLODBias[1] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; w = 1, MipMapLODBias[1] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceX, w = 1, MipMapLODBias[1] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceY, w = 1, MipMapLODBias[1] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; w = 1, MipMapLODBias[1] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceX, w = 1, MipMapLODBias[1] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceY, w = 1, MipMapLODBias[1] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; w = 1, MipMapLODBias[1] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceX, w = 1, MipMapLODBias[1] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceY, w = 1, MipMapLODBias[1] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; w = 1, MipMapLODBias[1] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceX, w = 1, MipMapLODBias[1] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceY, w = 1, MipMapLODBias[1] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; w = 1, MipMapLODBias[1] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceX, w = 1, MipMapLODBias[1] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceY, w = 1, MipMapLODBias[1] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; w = 1, MipMapLODBias[1] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceX, w = 1, MipMapLODBias[1] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol1; sliceY, w = 1, MipMapLODBias[1] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			texldp r1, v1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol1; w = 1, MipMapLODBias[1] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r0, v1
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol1; sliceX, w = 1, MipMapLODBias[1] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r0, v1
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol1; sliceY, w = 1, MipMapLODBias[1] = 0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r0, v1
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol1; w = 1, MipMapLODBias[1] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r1, v1
			texldp r1, r1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol1; sliceX, w = 1, MipMapLODBias[1] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r1, v1
			texldp r1, r1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol1; sliceY, w = 1, MipMapLODBias[1] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r1, v1
			texldp r1, r1, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol1; w = 1, MipMapLODBias[1] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r2, v1
			texldp r1, r2, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol1; sliceX, w = 1, MipMapLODBias[1] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r2, v1
			texldp r1, r2, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol1; sliceY, w = 1, MipMapLODBias[1] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r2, v1
			texldp r1, r2, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol1; w = 1, MipMapLODBias[1] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r3, v1
			texldp r1, r3, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol1; sliceX, w = 1, MipMapLODBias[1] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r3, v1
			texldp r1, r3, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol1; sliceY, w = 1, MipMapLODBias[1] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r3, v1
			texldp r1, r3, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol1; w = 1, MipMapLODBias[1] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r4, v1
			texldp r1, r4, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol1; sliceX, w = 1, MipMapLODBias[1] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r4, v1
			texldp r1, r4, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol1; sliceY, w = 1, MipMapLODBias[1] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r4, v1
			texldp r1, r4, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol1; w = 1, MipMapLODBias[1] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r5, v1
			texldp r1, r5, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol1; sliceX, w = 1, MipMapLODBias[1] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r5, v1
			texldp r1, r5, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol1; sliceY, w = 1, MipMapLODBias[1] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r5, v1
			texldp r1, r5, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol1; w = 1, MipMapLODBias[1] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r6, v1
			texldp r1, r6, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol1; sliceX, w = 1, MipMapLODBias[1] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r6, v1
			texldp r1, r6, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol1; sliceY, w = 1, MipMapLODBias[1] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s1
			
			mov r6, v1
			texldp r1, r6, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/32, MipMapLODBias[1] = -0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/32, MipMapLODBias[1] = -0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/32, MipMapLODBias[1]=-0";
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
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/32, MipMapLODBias[1] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/32, MipMapLODBias[1] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/32, MipMapLODBias[1]=-1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/32, MipMapLODBias[1] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/32, MipMapLODBias[1] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/32, MipMapLODBias[1]=-2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/32, MipMapLODBias[1] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/32, MipMapLODBias[1] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/32, MipMapLODBias[1]=-3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/32, MipMapLODBias[1] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/32, MipMapLODBias[1] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/32, MipMapLODBias[1]=-4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/32, MipMapLODBias[1] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/32, MipMapLODBias[1] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/32, MipMapLODBias[1]=-5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, w = 1/32, MipMapLODBias[1] = -6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceX, w = 1/32, MipMapLODBias[1] = -6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol1, sliceY, w = 1/32, MipMapLODBias[1]=-6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[1] = <TexVol_0>;
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s1
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol9";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r1, v1
			texldp r1, r1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol9, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r1, v1
			texldp r1, r1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol9, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r1, v1
			texldp r1, r1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c1, TexVol9";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.0f, 0.0f, 0.0f, 1.f
            dcl_volume s9
			
			texldp r1, c1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c1, TexVol9, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.0f, 0.0f, 0.48f, 1.f
            dcl_volume s9
			
			texldp r1, c1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c1, TexVol9, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.0f, 0.0f, 0.98f, 1.f
            dcl_volume s9
			
			texldp r1, c1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9, writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1.xyzw, v1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9, sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1.xyzw, v1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9, sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1.xyzw, v1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol9, writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r1, v1
			texldp r1.xyzw, r1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol9, sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r1, v1
			texldp r1.xyzw, r1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol9, sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r1, v1
			texldp r1.xyzw, r1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c1, TexVol9; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.0f, 0.0f, 0.98f, 1.f
            dcl_volume s9
			
			texldp r1.xyzw, c1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c1, TexVol9, sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.48f, 0.48f, 0.98f, 1.f
            dcl_volume s9
			
			texldp r1.xyzw, c1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c1, TexVol9, sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.98f, 0.98f, 0.98f, 1.f
            dcl_volume s9
			
			texldp r1.xyzw, c1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : v1, TexVol9";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp_pp r1, v1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : v1, TexVol9, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp_pp r1, v1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : v1, TexVol9, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp_pp r1, v1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : r1, TexVol9";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r1, v1
			texldp_pp r1, r1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : r1, TexVol9, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r1, v1
			texldp_pp r1, r1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : r1, TexVol9, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r1, v1
			texldp_pp r1, r1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : c1, TexVol9";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.48f, 0.48f, 0.0f, 1.f
            dcl_volume s9
			
			texldp_pp r1, c1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : c1, TexVol9, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.48f, 0.48f, 0.48f, 1.f
            dcl_volume s9
			
			texldp_pp r1, c1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : c1, TexVol9, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.48f, 0.48f, 0.98f, 1.f
            dcl_volume s9
			
			texldp_pp r1, c1, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 3.600000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.600000, 3.600000, 3.600000, 3.600000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 3.600000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.600000, 3.600000, 3.600000, 3.600000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 3.600000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.600000, 3.600000, 3.600000, 3.600000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 8.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 8.000000, 8.000000, 8.000000, 8.000000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 8.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 8.000000, 8.000000, 8.000000, 8.000000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 8.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 8.000000, 8.000000, 8.000000, 8.000000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = -5.600000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -5.600000, -5.600000, -5.600000, -5.600000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = -5.600000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -5.600000, -5.600000, -5.600000, -5.600000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = -5.600000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -5.600000, -5.600000, -5.600000, -5.600000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = -4.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -4.000000, -4.000000, -4.000000, -4.000000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = -4.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -4.000000, -4.000000, -4.000000, -4.000000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = -4.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -4.000000, -4.000000, -4.000000, -4.000000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/3.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.333333, 0.333333, 0.333333, 0.333333
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/3.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.333333, 0.333333, 0.333333, 0.333333
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/3.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.333333, 0.333333, 0.333333, 0.333333
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/-7.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.142857, -0.142857, -0.142857, -0.142857
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/-7.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.142857, -0.142857, -0.142857, -0.142857
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/-7.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.142857, -0.142857, -0.142857, -0.142857
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/8";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/8";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/8";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/16";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/16";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/16";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/32";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/32";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/32";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/64";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/64";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/64";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; w = 1, MaxMipLevel[9] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceX, w = 1, MaxMipLevel[9] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceY, w = 1, MaxMipLevel[9] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; w = 1, MaxMipLevel[9] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceX, w = 1, MaxMipLevel[9] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceY, w = 1, MaxMipLevel[9] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; w = 1, MaxMipLevel[9] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 2;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceX, w = 1, MaxMipLevel[9] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 2;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceY, w = 1, MaxMipLevel[9] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 2;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; w = 1, MaxMipLevel[9] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceX, w = 1, MaxMipLevel[9] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceY, w = 1, MaxMipLevel[9] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; w = 1, MaxMipLevel[9] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 4;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceX, w = 1, MaxMipLevel[9] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 4;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceY, w = 1, MaxMipLevel[9] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 4;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; w = 1, MaxMipLevel[9] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceX, w = 1, MaxMipLevel[9] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceY, w = 1, MaxMipLevel[9] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; w = 1, MaxMipLevel[9] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 6;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceX, w = 1, MaxMipLevel[9] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 6;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceY, w = 1, MaxMipLevel[9] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 6;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol9; w = 1, MaxMipLevel[9] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r0, v1
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol9; sliceX, w = 1, MaxMipLevel[9] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r0, v1
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol9; sliceY, w = 1, MaxMipLevel[9] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r0, v1
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol9; w = 1, MaxMipLevel[9] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r1, v1
			texldp r1, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol9; sliceX, w = 1, MaxMipLevel[9] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r1, v1
			texldp r1, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol9; sliceY, w = 1, MaxMipLevel[9] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r1, v1
			texldp r1, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol9; w = 1, MaxMipLevel[9] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 2;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r2, v1
			texldp r1, r2, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol9; sliceX, w = 1, MaxMipLevel[9] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 2;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r2, v1
			texldp r1, r2, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol9; sliceY, w = 1, MaxMipLevel[9] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 2;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r2, v1
			texldp r1, r2, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol9; w = 1, MaxMipLevel[9] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r3, v1
			texldp r1, r3, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol9; sliceX, w = 1, MaxMipLevel[9] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r3, v1
			texldp r1, r3, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol9; sliceY, w = 1, MaxMipLevel[9] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r3, v1
			texldp r1, r3, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol9; w = 1, MaxMipLevel[9] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 4;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r4, v1
			texldp r1, r4, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol9; sliceX, w = 1, MaxMipLevel[9] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 4;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r4, v1
			texldp r1, r4, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol9; sliceY, w = 1, MaxMipLevel[9] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 4;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r4, v1
			texldp r1, r4, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol9; w = 1, MaxMipLevel[9] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r5, v1
			texldp r1, r5, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol9; sliceX, w = 1, MaxMipLevel[9] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r5, v1
			texldp r1, r5, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol9; sliceY, w = 1, MaxMipLevel[9] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r5, v1
			texldp r1, r5, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol9; w = 1, MaxMipLevel[9] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 6;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r6, v1
			texldp r1, r6, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol9; sliceX, w = 1, MaxMipLevel[9] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 6;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r6, v1
			texldp r1, r6, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol9; sliceY, w = 1, MaxMipLevel[9] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 6;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r6, v1
			texldp r1, r6, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; w = 1, MipMapLODBias[9] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceX, w = 1, MipMapLODBias[9] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceY, w = 1, MipMapLODBias[9] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; w = 1, MipMapLODBias[9] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceX, w = 1, MipMapLODBias[9] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceY, w = 1, MipMapLODBias[9] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; w = 1, MipMapLODBias[9] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceX, w = 1, MipMapLODBias[9] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceY, w = 1, MipMapLODBias[9] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; w = 1, MipMapLODBias[9] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceX, w = 1, MipMapLODBias[9] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceY, w = 1, MipMapLODBias[9] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; w = 1, MipMapLODBias[9] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceX, w = 1, MipMapLODBias[9] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceY, w = 1, MipMapLODBias[9] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; w = 1, MipMapLODBias[9] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceX, w = 1, MipMapLODBias[9] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceY, w = 1, MipMapLODBias[9] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; w = 1, MipMapLODBias[9] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceX, w = 1, MipMapLODBias[9] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol9; sliceY, w = 1, MipMapLODBias[9] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			texldp r1, v1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol9; w = 1, MipMapLODBias[9] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r0, v1
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol9; sliceX, w = 1, MipMapLODBias[9] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r0, v1
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol9; sliceY, w = 1, MipMapLODBias[9] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r0, v1
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol9; w = 1, MipMapLODBias[9] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r1, v1
			texldp r1, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol9; sliceX, w = 1, MipMapLODBias[9] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r1, v1
			texldp r1, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol9; sliceY, w = 1, MipMapLODBias[9] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r1, v1
			texldp r1, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol9; w = 1, MipMapLODBias[9] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r2, v1
			texldp r1, r2, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol9; sliceX, w = 1, MipMapLODBias[9] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r2, v1
			texldp r1, r2, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol9; sliceY, w = 1, MipMapLODBias[9] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r2, v1
			texldp r1, r2, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol9; w = 1, MipMapLODBias[9] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r3, v1
			texldp r1, r3, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol9; sliceX, w = 1, MipMapLODBias[9] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r3, v1
			texldp r1, r3, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol9; sliceY, w = 1, MipMapLODBias[9] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r3, v1
			texldp r1, r3, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol9; w = 1, MipMapLODBias[9] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r4, v1
			texldp r1, r4, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol9; sliceX, w = 1, MipMapLODBias[9] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r4, v1
			texldp r1, r4, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol9; sliceY, w = 1, MipMapLODBias[9] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r4, v1
			texldp r1, r4, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol9; w = 1, MipMapLODBias[9] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r5, v1
			texldp r1, r5, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol9; sliceX, w = 1, MipMapLODBias[9] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r5, v1
			texldp r1, r5, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol9; sliceY, w = 1, MipMapLODBias[9] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r5, v1
			texldp r1, r5, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol9; w = 1, MipMapLODBias[9] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r6, v1
			texldp r1, r6, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol9; sliceX, w = 1, MipMapLODBias[9] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r6, v1
			texldp r1, r6, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol9; sliceY, w = 1, MipMapLODBias[9] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s9
			
			mov r6, v1
			texldp r1, r6, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/32, MipMapLODBias[9] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/32, MipMapLODBias[9] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/32, MipMapLODBias[9]=-0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/32, MipMapLODBias[9] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/32, MipMapLODBias[9] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/32, MipMapLODBias[9]=-1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/32, MipMapLODBias[9] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/32, MipMapLODBias[9] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/32, MipMapLODBias[9]=-2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/32, MipMapLODBias[9] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/32, MipMapLODBias[9] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/32, MipMapLODBias[9]=-3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/32, MipMapLODBias[9] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/32, MipMapLODBias[9] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/32, MipMapLODBias[9]=-4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/32, MipMapLODBias[9] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/32, MipMapLODBias[9] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/32, MipMapLODBias[9]=-5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, w = 1/32, MipMapLODBias[9] = -6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceX, w = 1/32, MipMapLODBias[9] = -6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol9, sliceY, w = 1/32, MipMapLODBias[9]=-6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[9] = <TexVol_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s9
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v2, TexVol14";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			texldp r1, v2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v2, TexVol14, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			texldp r1, v2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v2, TexVol14, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			texldp r1, v2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol14";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			mov r2, v2
			texldp r1, r2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol14, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			mov r2, v2
			texldp r1, r2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol14, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			mov r2, v2
			texldp r1, r2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c2, TexVol14";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.0f, 0.0f, 0.0f, 2.f
            dcl_volume s14
			
			texldp r1, c2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c2, TexVol14, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.0f, 0.0f, 0.48f, 2.f
            dcl_volume s14
			
			texldp r1, c2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c2, TexVol14, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.0f, 0.0f, 0.98f, 2.f
            dcl_volume s14
			
			texldp r1, c2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v2, TexVol14, writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			texldp r1.xyzw, v2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v2, TexVol14, sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			texldp r1.xyzw, v2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v2, TexVol14, sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			texldp r1.xyzw, v2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol14, writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			mov r2, v2
			texldp r1.xyzw, r2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol14, sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			mov r2, v2
			texldp r1.xyzw, r2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol14, sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			mov r2, v2
			texldp r1.xyzw, r2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c2, TexVol14; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.0f, 0.0f, 0.98f, 2.f
            dcl_volume s14
			
			texldp r1.xyzw, c2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c2, TexVol14, sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.48f, 0.48f, 0.98f, 2.f
            dcl_volume s14
			
			texldp r1.xyzw, c2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c2, TexVol14, sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.98f, 0.98f, 0.98f, 2.f
            dcl_volume s14
			
			texldp r1.xyzw, c2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : v2, TexVol14";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			texldp_pp r1, v2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : v2, TexVol14, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			texldp_pp r1, v2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : v2, TexVol14, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			texldp_pp r1, v2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : r2, TexVol14";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			mov r2, v2
			texldp_pp r1, r2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : r2, TexVol14, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			mov r2, v2
			texldp_pp r1, r2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : r2, TexVol14, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s14
			
			mov r2, v2
			texldp_pp r1, r2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : c2, TexVol14";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.48f, 0.48f, 0.0f, 2.f
            dcl_volume s14
			
			texldp_pp r1, c2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : c2, TexVol14, sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.48f, 0.48f, 0.48f, 2.f
            dcl_volume s14
			
			texldp_pp r1, c2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : c2, TexVol14, sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.48f, 0.48f, 0.98f, 2.f
            dcl_volume s14
			
			texldp_pp r1, c2, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 5.400000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.400000, 5.400000, 5.400000, 5.400000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 5.400000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.400000, 5.400000, 5.400000, 5.400000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 5.400000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.400000, 5.400000, 5.400000, 5.400000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 12.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 12.000000, 12.000000, 12.000000, 12.000000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 12.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 12.000000, 12.000000, 12.000000, 12.000000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 12.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 12.000000, 12.000000, 12.000000, 12.000000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = -8.400000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -8.400000, -8.400000, -8.400000, -8.400000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = -8.400000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -8.400000, -8.400000, -8.400000, -8.400000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = -8.400000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -8.400000, -8.400000, -8.400000, -8.400000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = -6.000000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -6.000000, -6.000000, -6.000000, -6.000000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = -6.000000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -6.000000, -6.000000, -6.000000, -6.000000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = -6.000000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -6.000000, -6.000000, -6.000000, -6.000000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/4.500000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.222222, 0.222222, 0.222222, 0.222222
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/4.500000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.222222, 0.222222, 0.222222, 0.222222
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/4.500000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.222222, 0.222222, 0.222222, 0.222222
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/-10.500000";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.095238, -0.095238, -0.095238, -0.095238
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/-10.500000";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.095238, -0.095238, -0.095238, -0.095238
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/-10.500000";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = None;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, -0.095238, -0.095238, -0.095238, -0.095238
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/8";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/8";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/8";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/16";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/16";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/16";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/32";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/32";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/32";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/64";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/64";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/64";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; w = 1, MaxMipLevel[14] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceX, w = 1, MaxMipLevel[14] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceY, w = 1, MaxMipLevel[14] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; w = 1, MaxMipLevel[14] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 1;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceX, w = 1, MaxMipLevel[14] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 1;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceY, w = 1, MaxMipLevel[14] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 1;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; w = 1, MaxMipLevel[14] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceX, w = 1, MaxMipLevel[14] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceY, w = 1, MaxMipLevel[14] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; w = 1, MaxMipLevel[14] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 3;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceX, w = 1, MaxMipLevel[14] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 3;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceY, w = 1, MaxMipLevel[14] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 3;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; w = 1, MaxMipLevel[14] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 4;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceX, w = 1, MaxMipLevel[14] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 4;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceY, w = 1, MaxMipLevel[14] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 4;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; w = 1, MaxMipLevel[14] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 5;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceX, w = 1, MaxMipLevel[14] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 5;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceY, w = 1, MaxMipLevel[14] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 5;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; w = 1, MaxMipLevel[14] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 6;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceX, w = 1, MaxMipLevel[14] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 6;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceY, w = 1, MaxMipLevel[14] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 6;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol14; w = 1, MaxMipLevel[14] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r0, v1
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol14; sliceX, w = 1, MaxMipLevel[14] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r0, v1
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol14; sliceY, w = 1, MaxMipLevel[14] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r0, v1
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol14; w = 1, MaxMipLevel[14] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 1;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r1, v1
			texldp r1, r1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol14; sliceX, w = 1, MaxMipLevel[14] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 1;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r1, v1
			texldp r1, r1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol14; sliceY, w = 1, MaxMipLevel[14] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 1;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r1, v1
			texldp r1, r1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol14; w = 1, MaxMipLevel[14] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r2, v1
			texldp r1, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol14; sliceX, w = 1, MaxMipLevel[14] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r2, v1
			texldp r1, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol14; sliceY, w = 1, MaxMipLevel[14] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r2, v1
			texldp r1, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol14; w = 1, MaxMipLevel[14] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 3;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r3, v1
			texldp r1, r3, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol14; sliceX, w = 1, MaxMipLevel[14] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 3;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r3, v1
			texldp r1, r3, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol14; sliceY, w = 1, MaxMipLevel[14] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 3;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r3, v1
			texldp r1, r3, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol14; w = 1, MaxMipLevel[14] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 4;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r4, v1
			texldp r1, r4, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol14; sliceX, w = 1, MaxMipLevel[14] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 4;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r4, v1
			texldp r1, r4, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol14; sliceY, w = 1, MaxMipLevel[14] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 4;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r4, v1
			texldp r1, r4, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol14; w = 1, MaxMipLevel[14] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 5;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r5, v1
			texldp r1, r5, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol14; sliceX, w = 1, MaxMipLevel[14] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 5;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r5, v1
			texldp r1, r5, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol14; sliceY, w = 1, MaxMipLevel[14] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 5;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r5, v1
			texldp r1, r5, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol14; w = 1, MaxMipLevel[14] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 6;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r6, v1
			texldp r1, r6, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol14; sliceX, w = 1, MaxMipLevel[14] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 6;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r6, v1
			texldp r1, r6, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol14; sliceY, w = 1, MaxMipLevel[14] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 6;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r6, v1
			texldp r1, r6, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; w = 1, MipMapLODBias[14] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceX, w = 1, MipMapLODBias[14] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceY, w = 1, MipMapLODBias[14] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; w = 1, MipMapLODBias[14] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceX, w = 1, MipMapLODBias[14] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceY, w = 1, MipMapLODBias[14] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; w = 1, MipMapLODBias[14] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceX, w = 1, MipMapLODBias[14] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceY, w = 1, MipMapLODBias[14] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; w = 1, MipMapLODBias[14] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceX, w = 1, MipMapLODBias[14] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceY, w = 1, MipMapLODBias[14] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; w = 1, MipMapLODBias[14] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceX, w = 1, MipMapLODBias[14] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceY, w = 1, MipMapLODBias[14] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; w = 1, MipMapLODBias[14] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceX, w = 1, MipMapLODBias[14] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceY, w = 1, MipMapLODBias[14] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; w = 1, MipMapLODBias[14] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceX, w = 1, MipMapLODBias[14] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, TexVol14; sliceY, w = 1, MipMapLODBias[14] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			texldp r1, v1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol14; w = 1, MipMapLODBias[14] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r0, v1
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol14; sliceX, w = 1, MipMapLODBias[14] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r0, v1
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, TexVol14; sliceY, w = 1, MipMapLODBias[14] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r0, v1
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol14; w = 1, MipMapLODBias[14] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r1, v1
			texldp r1, r1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol14; sliceX, w = 1, MipMapLODBias[14] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r1, v1
			texldp r1, r1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, TexVol14; sliceY, w = 1, MipMapLODBias[14] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r1, v1
			texldp r1, r1, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol14; w = 1, MipMapLODBias[14] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r2, v1
			texldp r1, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol14; sliceX, w = 1, MipMapLODBias[14] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r2, v1
			texldp r1, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, TexVol14; sliceY, w = 1, MipMapLODBias[14] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r2, v1
			texldp r1, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol14; w = 1, MipMapLODBias[14] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r3, v1
			texldp r1, r3, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol14; sliceX, w = 1, MipMapLODBias[14] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r3, v1
			texldp r1, r3, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, TexVol14; sliceY, w = 1, MipMapLODBias[14] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r3, v1
			texldp r1, r3, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol14; w = 1, MipMapLODBias[14] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r4, v1
			texldp r1, r4, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol14; sliceX, w = 1, MipMapLODBias[14] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r4, v1
			texldp r1, r4, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, TexVol14; sliceY, w = 1, MipMapLODBias[14] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r4, v1
			texldp r1, r4, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol14; w = 1, MipMapLODBias[14] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r5, v1
			texldp r1, r5, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol14; sliceX, w = 1, MipMapLODBias[14] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r5, v1
			texldp r1, r5, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, TexVol14; sliceY, w = 1, MipMapLODBias[14] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r5, v1
			texldp r1, r5, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol14; w = 1, MipMapLODBias[14] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r6, v1
			texldp r1, r6, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol14; sliceX, w = 1, MipMapLODBias[14] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r6, v1
			texldp r1, r6, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, TexVol14; sliceY, w = 1, MipMapLODBias[14] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s14
			
			mov r6, v1
			texldp r1, r6, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/32, MipMapLODBias[14] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/32, MipMapLODBias[14] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/32, MipMapLODBias[14]=-0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/32, MipMapLODBias[14] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/32, MipMapLODBias[14] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/32, MipMapLODBias[14]=-1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/32, MipMapLODBias[14] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/32, MipMapLODBias[14] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/32, MipMapLODBias[14]=-2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/32, MipMapLODBias[14] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/32, MipMapLODBias[14] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/32, MipMapLODBias[14]=-3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/32, MipMapLODBias[14] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/32, MipMapLODBias[14] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/32, MipMapLODBias[14]=-4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/32, MipMapLODBias[14] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/32, MipMapLODBias[14] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/32, MipMapLODBias[14]=-5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, w = 1/32, MipMapLODBias[14] = -6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceX, w = 1/32, MipMapLODBias[14] = -6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : TexVol14, sliceY, w = 1/32, MipMapLODBias[14]=-6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[14] = <TexVol_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_volume s14
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
