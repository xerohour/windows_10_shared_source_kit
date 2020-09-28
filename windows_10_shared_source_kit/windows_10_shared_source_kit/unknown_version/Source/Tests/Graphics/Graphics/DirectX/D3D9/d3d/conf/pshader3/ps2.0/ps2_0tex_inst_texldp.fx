#include "include.fx"

Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, writemask = xyzw";
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
			ps_2_0
			dcl t0.xy
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1.xyzw, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1.500000";
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
			ps_2_0
			dcl t0.xy
			def  c0, 1.500000, 1.500000, 1.500000, 1.500000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 3.000000";
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
			ps_2_0
			dcl t0.xy
			def  c0, 3.000000, 3.000000, 3.000000, 3.000000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = -2.000000";
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
			ps_2_0
			dcl t0.xy
			def  c0, -2.000000, -2.000000, -2.000000, -2.000000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = -2.500000";
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
			ps_2_0
			dcl t0.xy
			def  c0, -2.500000, -2.500000, -2.500000, -2.500000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/2.000000";
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
			ps_2_0
			dcl t0.xy
			def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/-3.000000";
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
			ps_2_0
			dcl t0.xy
			def  c0, -0.333333, -0.333333, -0.333333, -0.333333
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/1";
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
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/2";
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
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/4";
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
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/8";
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
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/16";
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
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32";
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
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/64";
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
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MaxMipLevel[0] = 0";
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
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MaxMipLevel[0] = 1";
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
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MaxMipLevel[0] = 2";
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
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MaxMipLevel[0] = 3";
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
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MaxMipLevel[0] = 4";
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
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MaxMipLevel[0] = 5";
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
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MaxMipLevel[0] = 6";
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
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MipMapLODBias[0] = 0";
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
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MipMapLODBias[0] = 1";
	String	Shape = "TexTL";
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
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MipMapLODBias[0] = 2";
	String	Shape = "TexTL";
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
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MipMapLODBias[0] = 3";
	String	Shape = "TexTL";
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
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MipMapLODBias[0] = 4";
	String	Shape = "TexTL";
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
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MipMapLODBias[0] = 5";
	String	Shape = "TexTL";
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
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MipMapLODBias[0] = 6";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32, MipMapLODBias[0] = -0";
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32, MipMapLODBias[0] = -1";
	String	Shape = "TexTL";
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32, MipMapLODBias[0] = -2";
	String	Shape = "TexTL";
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32, MipMapLODBias[0] = -3";
	String	Shape = "TexTL";
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32, MipMapLODBias[0] = -4";
	String	Shape = "TexTL";
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32, MipMapLODBias[0] = -5";
	String	Shape = "TexTL";
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
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32, MipMapLODBias[0] = -6";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
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
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1.xyzw, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 3.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 3.000000, 3.000000, 3.000000, 3.000000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 6.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 6.000000, 6.000000, 6.000000, 6.000000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = -4.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -4.000000, -4.000000, -4.000000, -4.000000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = -5.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -5.000000, -5.000000, -5.000000, -5.000000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/4.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/-6.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -0.166667, -0.166667, -0.166667, -0.166667
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/8";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/16";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/32";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/64";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MaxMipLevel[9] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MaxMipLevel[9] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MaxMipLevel[9] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 2;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MaxMipLevel[9] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MaxMipLevel[9] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 4;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MaxMipLevel[9] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MaxMipLevel[9] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 6;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MipMapLODBias[9] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MipMapLODBias[9] = 1";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MipMapLODBias[9] = 2";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MipMapLODBias[9] = 3";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MipMapLODBias[9] = 4";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MipMapLODBias[9] = 5";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MipMapLODBias[9] = 6";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/32, MipMapLODBias[9] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/32, MipMapLODBias[9] = -1";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/32, MipMapLODBias[9] = -2";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/32, MipMapLODBias[9] = -3";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/32, MipMapLODBias[9] = -4";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/32, MipMapLODBias[9] = -5";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/32, MipMapLODBias[9] = -6";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1, writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1.xyzw, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 4.500000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 4.500000, 4.500000, 4.500000, 4.500000
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 9.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 9.000000, 9.000000, 9.000000, 9.000000
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = -6.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -6.000000, -6.000000, -6.000000, -6.000000
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = -7.500000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -7.500000, -7.500000, -7.500000, -7.500000
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/6.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 0.166667, 0.166667, 0.166667, 0.166667
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/-9.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -0.111111, -0.111111, -0.111111, -0.111111
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/8";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/16";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/32";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/64";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1, MaxMipLevel[14] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1, MaxMipLevel[14] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 1;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1, MaxMipLevel[14] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 2;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1, MaxMipLevel[14] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 3;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1, MaxMipLevel[14] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 4;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1, MaxMipLevel[14] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 5;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1, MaxMipLevel[14] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
		MipFilter[14] = Point;
        MaxMipLevel[14] = 6;
        MipMapLODBias[14] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1, MipMapLODBias[14] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1, MipMapLODBias[14] = 1";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1, MipMapLODBias[14] = 2";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1, MipMapLODBias[14] = 3";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1, MipMapLODBias[14] = 4";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1, MipMapLODBias[14] = 5";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1, MipMapLODBias[14] = 6";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/32, MipMapLODBias[14] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/32, MipMapLODBias[14] = -1";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/32, MipMapLODBias[14] = -2";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/32, MipMapLODBias[14] = -3";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/32, MipMapLODBias[14] = -4";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/32, MipMapLODBias[14] = -5";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex14, w = 1/32, MipMapLODBias[14] = -6";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[14] = <Tex2D_0>;
 		MipFilter[14] = Point;
        MaxMipLevel[14] = 0;
        MipMapLODBias[14] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s14
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
