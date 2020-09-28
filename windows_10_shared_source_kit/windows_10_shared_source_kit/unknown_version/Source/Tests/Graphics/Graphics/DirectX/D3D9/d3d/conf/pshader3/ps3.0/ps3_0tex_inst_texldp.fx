#include "include.fx"

Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = 1.500000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.500000, 1.500000, 1.500000, 1.500000
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = 3.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 3.000000, 3.000000, 3.000000, 3.000000
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = -2.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -2.000000, -2.000000, -2.000000, -2.000000
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = -2.500000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -2.500000, -2.500000, -2.500000, -2.500000
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = 1/2.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = 1/-3.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -0.333333, -0.333333, -0.333333, -0.333333
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = 1/1";
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
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = 1/2";
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
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = 1/4";
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
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = 1/8";
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
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = 1/16";
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
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = 1/32";
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
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = 1/64";
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
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex0; w = 1, MaxMipLevel[0] = 0";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			texldp r1, v1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex0; w = 1, MaxMipLevel[0] = 1";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			texldp r1, v1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex0; w = 1, MaxMipLevel[0] = 2";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			texldp r1, v1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex0; w = 1, MaxMipLevel[0] = 3";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			texldp r1, v1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex0; w = 1, MaxMipLevel[0] = 4";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			texldp r1, v1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex0; w = 1, MaxMipLevel[0] = 5";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			texldp r1, v1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex0; w = 1, MaxMipLevel[0] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 6;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			texldp r1, v1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = 1, MaxMipLevel[0] = 0";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			mov r0, v1
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, Tex0; w = 1, MaxMipLevel[0] = 1";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			mov r1, v1
			texldp r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, Tex0; w = 1, MaxMipLevel[0] = 2";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			mov r2, v1
			texldp r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, Tex0; w = 1, MaxMipLevel[0] = 3";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			mov r3, v1
			texldp r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, Tex0; w = 1, MaxMipLevel[0] = 4";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			mov r4, v1
			texldp r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, Tex0; w = 1, MaxMipLevel[0] = 5";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			mov r5, v1
			texldp r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, Tex0; w = 1, MaxMipLevel[0] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 6;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			mov r6, v1
			texldp r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex0; w = 1, MipMapLODBias[0] = 0";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			texldp r1, v1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex0; w = 1, MipMapLODBias[0] = 1";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			texldp r1, v1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex0; w = 1, MipMapLODBias[0] = 2";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			texldp r1, v1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex0; w = 1, MipMapLODBias[0] = 3";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			texldp r1, v1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex0; w = 1, MipMapLODBias[0] = 4";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			texldp r1, v1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex0; w = 1, MipMapLODBias[0] = 5";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			texldp r1, v1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex0; w = 1, MipMapLODBias[0] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			texldp r1, v1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = 1, MipMapLODBias[0] = 0";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			mov r0, v1
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, Tex0; w = 1, MipMapLODBias[0] = 1";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			mov r1, v1
			texldp r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, Tex0; w = 1, MipMapLODBias[0] = 2";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			mov r2, v1
			texldp r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, Tex0; w = 1, MipMapLODBias[0] = 3";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			mov r3, v1
			texldp r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, Tex0; w = 1, MipMapLODBias[0] = 4";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			mov r4, v1
			texldp r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, Tex0; w = 1, MipMapLODBias[0] = 5";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			mov r5, v1
			texldp r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, Tex0; w = 1, MipMapLODBias[0] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s0
			
			mov r6, v1
			texldp r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex0; w = 1/32, MipMapLODBias[0] = -0";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, Tex0; w = 1/32, MipMapLODBias[0] = -1";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r1, v0.xy
			mov r1.w, c0.x
			texldp r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, Tex0; w = 1/32, MipMapLODBias[0] = -2";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r2, v0.xy
			mov r2.w, c0.x
			texldp r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, Tex0; w = 1/32, MipMapLODBias[0] = -3";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r3, v0.xy
			mov r3.w, c0.x
			texldp r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, Tex0; w = 1/32, MipMapLODBias[0] = -4";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r4, v0.xy
			mov r4.w, c0.x
			texldp r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, Tex0; w = 1/32, MipMapLODBias[0] = -5";
	String	Shape = "TexTL";
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
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r5, v0.xy
			mov r5.w, c0.x
			texldp r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, Tex0; w = 1/32, MipMapLODBias[0] = -6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r6, v0.xy
			mov r6.w, c0.x
			texldp r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			texldp r0, v1, s7
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1(.xyw), Tex7";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1.xyw
            dcl_2d s7
			
			texldp r0, v1, s7
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, Tex7";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			mov r1, v1
			texldp r0, r1, s7
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c1, Tex7";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.0f, 0.0f, 0.0f, 1.f
            dcl_2d s7
			
			texldp r0, c1, s7
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7; writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_2d s7
			
			texldp r1.xyzw, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, Tex7; writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_2d s7
			
			mov r1, v1
			texldp r1.xyzw, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c1, Tex7; writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.98f, 0.98f, 0.98f, 1.f
            dcl_2d s7
			
			texldp r0.xyzw, c1, s7
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : v1, Tex7";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_2d s7
			
			texldp_pp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : r1, Tex7";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_2d s7
			
			mov r1, v1
			texldp_pp r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : c1, Tex7";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.48f, 0.48f, 0.48f, 1.f
            dcl_2d s7
			
			texldp_pp r0, c1, s7
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = 3.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 3.000000, 3.000000, 3.000000, 3.000000
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = 6.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 6.000000, 6.000000, 6.000000, 6.000000
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = -4.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -4.000000, -4.000000, -4.000000, -4.000000
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = -5.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -5.000000, -5.000000, -5.000000, -5.000000
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = 1/4.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = 1/-6.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -0.166667, -0.166667, -0.166667, -0.166667
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = 1/1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = 1/2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = 1/4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = 1/8";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = 1/16";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = 1/32";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = 1/64";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7; w = 1, MaxMipLevel[7] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			texldp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7; w = 1, MaxMipLevel[7] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			texldp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7; w = 1, MaxMipLevel[7] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			texldp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7; w = 1, MaxMipLevel[7] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			texldp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7; w = 1, MaxMipLevel[7] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			texldp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7; w = 1, MaxMipLevel[7] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			texldp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7; w = 1, MaxMipLevel[7] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			texldp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = 1, MaxMipLevel[7] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			mov r0, v1
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, Tex7; w = 1, MaxMipLevel[7] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			mov r1, v1
			texldp r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, Tex7; w = 1, MaxMipLevel[7] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			mov r2, v1
			texldp r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, Tex7; w = 1, MaxMipLevel[7] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			mov r3, v1
			texldp r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, Tex7; w = 1, MaxMipLevel[7] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			mov r4, v1
			texldp r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, Tex7; w = 1, MaxMipLevel[7] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			mov r5, v1
			texldp r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, Tex7; w = 1, MaxMipLevel[7] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			mov r6, v1
			texldp r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7; w = 1, MipMapLODBias[7] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			texldp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7; w = 1, MipMapLODBias[7] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			texldp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7; w = 1, MipMapLODBias[7] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			texldp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7; w = 1, MipMapLODBias[7] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			texldp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7; w = 1, MipMapLODBias[7] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			texldp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7; w = 1, MipMapLODBias[7] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			texldp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex7; w = 1, MipMapLODBias[7] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			texldp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = 1, MipMapLODBias[7] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			mov r0, v1
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, Tex7; w = 1, MipMapLODBias[7] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			mov r1, v1
			texldp r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, Tex7; w = 1, MipMapLODBias[7] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			mov r2, v1
			texldp r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, Tex7; w = 1, MipMapLODBias[7] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			mov r3, v1
			texldp r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, Tex7; w = 1, MipMapLODBias[7] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			mov r4, v1
			texldp r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, Tex7; w = 1, MipMapLODBias[7] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			mov r5, v1
			texldp r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, Tex7; w = 1, MipMapLODBias[7] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s7
			
			mov r6, v1
			texldp r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex7; w = 1/32, MipMapLODBias[7] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s7
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, Tex7; w = 1/32, MipMapLODBias[7] = -1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s7
			
			mov r1, v0.xy
			mov r1.w, c0.x
			texldp r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, Tex7; w = 1/32, MipMapLODBias[7] = -2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s7
			
			mov r2, v0.xy
			mov r2.w, c0.x
			texldp r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, Tex7; w = 1/32, MipMapLODBias[7] = -3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s7
			
			mov r3, v0.xy
			mov r3.w, c0.x
			texldp r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, Tex7; w = 1/32, MipMapLODBias[7] = -4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s7
			
			mov r4, v0.xy
			mov r4.w, c0.x
			texldp r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, Tex7; w = 1/32, MipMapLODBias[7] = -5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s7
			
			mov r5, v0.xy
			mov r5.w, c0.x
			texldp r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, Tex7; w = 1/32, MipMapLODBias[7] = -6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[7] = <Tex2D_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s7
			
			mov r6, v0.xy
			mov r6.w, c0.x
			texldp r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v2, Tex12";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s12
			
			texldp r0, v2, s12
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v2(.xyw), Tex12";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2.xyw
            dcl_2d s12
			
			texldp r0, v2, s12
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, Tex12";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_2d s12
			
			mov r2, v2
			texldp r0, r2, s12
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c2, Tex12";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.0f, 0.0f, 0.0f, 2.f
            dcl_2d s12
			
			texldp r0, c2, s12
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v2, Tex12; writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_2d s12
			
			texldp r1.xyzw, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, Tex12; writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_2d s12
			
			mov r2, v2
			texldp r1.xyzw, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : c2, Tex12; writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.98f, 0.98f, 0.98f, 2.f
            dcl_2d s12
			
			texldp r0.xyzw, c2, s12
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : v2, Tex12";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_2d s12
			
			texldp_pp r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : r2, Tex12";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_2d s12
			
			mov r2, v2
			texldp_pp r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp_pp : c2, Tex12";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.48f, 0.48f, 0.48f, 2.f
            dcl_2d s12
			
			texldp_pp r0, c2, s12
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = 4.500000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.500000, 4.500000, 4.500000, 4.500000
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = 9.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 9.000000, 9.000000, 9.000000, 9.000000
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = -6.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -6.000000, -6.000000, -6.000000, -6.000000
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = -7.500000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -7.500000, -7.500000, -7.500000, -7.500000
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = 1/6.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0.166667, 0.166667, 0.166667, 0.166667
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = 1/-9.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, -0.111111, -0.111111, -0.111111, -0.111111
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = 1/1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = 1/2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = 1/4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = 1/8";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = 1/16";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = 1/32";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = 1/64";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex12; w = 1, MaxMipLevel[12] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			texldp r1, v1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex12; w = 1, MaxMipLevel[12] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			texldp r1, v1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex12; w = 1, MaxMipLevel[12] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			texldp r1, v1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex12; w = 1, MaxMipLevel[12] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			texldp r1, v1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex12; w = 1, MaxMipLevel[12] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			texldp r1, v1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex12; w = 1, MaxMipLevel[12] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			texldp r1, v1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex12; w = 1, MaxMipLevel[12] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			texldp r1, v1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = 1, MaxMipLevel[12] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			mov r0, v1
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, Tex12; w = 1, MaxMipLevel[12] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			mov r1, v1
			texldp r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, Tex12; w = 1, MaxMipLevel[12] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			mov r2, v1
			texldp r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, Tex12; w = 1, MaxMipLevel[12] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			mov r3, v1
			texldp r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, Tex12; w = 1, MaxMipLevel[12] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			mov r4, v1
			texldp r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, Tex12; w = 1, MaxMipLevel[12] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			mov r5, v1
			texldp r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, Tex12; w = 1, MaxMipLevel[12] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			mov r6, v1
			texldp r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex12; w = 1, MipMapLODBias[12] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			texldp r1, v1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex12; w = 1, MipMapLODBias[12] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			texldp r1, v1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex12; w = 1, MipMapLODBias[12] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			texldp r1, v1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex12; w = 1, MipMapLODBias[12] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			texldp r1, v1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex12; w = 1, MipMapLODBias[12] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			texldp r1, v1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex12; w = 1, MipMapLODBias[12] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			texldp r1, v1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : v1, Tex12; w = 1, MipMapLODBias[12] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			texldp r1, v1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = 1, MipMapLODBias[12] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			mov r0, v1
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, Tex12; w = 1, MipMapLODBias[12] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			mov r1, v1
			texldp r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, Tex12; w = 1, MipMapLODBias[12] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			mov r2, v1
			texldp r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, Tex12; w = 1, MipMapLODBias[12] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			mov r3, v1
			texldp r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, Tex12; w = 1, MipMapLODBias[12] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			mov r4, v1
			texldp r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, Tex12; w = 1, MipMapLODBias[12] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			mov r5, v1
			texldp r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, Tex12; w = 1, MipMapLODBias[12] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_2d s12
			
			mov r6, v1
			texldp r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r0, Tex12; w = 1/32, MipMapLODBias[12] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s12
			
			mov r0, v0.xy
			mov r0.w, c0.x
			texldp r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r1, Tex12; w = 1/32, MipMapLODBias[12] = -1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s12
			
			mov r1, v0.xy
			mov r1.w, c0.x
			texldp r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r2, Tex12; w = 1/32, MipMapLODBias[12] = -2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s12
			
			mov r2, v0.xy
			mov r2.w, c0.x
			texldp r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r3, Tex12; w = 1/32, MipMapLODBias[12] = -3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s12
			
			mov r3, v0.xy
			mov r3.w, c0.x
			texldp r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r4, Tex12; w = 1/32, MipMapLODBias[12] = -4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s12
			
			mov r4, v0.xy
			mov r4.w, c0.x
			texldp r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r5, Tex12; w = 1/32, MipMapLODBias[12] = -5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s12
			
			mov r5, v0.xy
			mov r5.w, c0.x
			texldp r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldp : r6, Tex12; w = 1/32, MipMapLODBias[12] = -6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[12] = <Tex2D_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s12
			
			mov r6, v0.xy
			mov r6.w, c0.x
			texldp r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
