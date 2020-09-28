#include "include.fx"

Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = 1.500000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 1.500000
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = 3.000000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 3.000000
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = -2.000000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -2.000000
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = -2.500000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -2.500000
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = 1/2.000000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 0.500000
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = 1/-3.000000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -0.333333
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = 1/1";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 1.000000
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = 1/2";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.500000
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = 1/4";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.250000
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = 1/8";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.125000
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = 1/16";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.062500
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = 1/32";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.031250
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = 1/64";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.015625
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex0; w = 1, MaxMipLevel[0] = 0";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			texldp r1, t1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex0; w = 1, MaxMipLevel[0] = 1";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			texldp r1, t1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex0; w = 1, MaxMipLevel[0] = 2";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			texldp r1, t1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex0; w = 1, MaxMipLevel[0] = 3";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			texldp r1, t1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex0; w = 1, MaxMipLevel[0] = 4";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			texldp r1, t1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex0; w = 1, MaxMipLevel[0] = 5";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			texldp r1, t1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex0; w = 1, MaxMipLevel[0] = 6";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			texldp r1, t1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = 1, MaxMipLevel[0] = 0";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			mov r0, t1
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, Tex0; w = 1, MaxMipLevel[0] = 1";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			mov r1, t1
			texldp r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, Tex0; w = 1, MaxMipLevel[0] = 2";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			mov r2, t1
			texldp r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, Tex0; w = 1, MaxMipLevel[0] = 3";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			mov r3, t1
			texldp r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, Tex0; w = 1, MaxMipLevel[0] = 4";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			mov r4, t1
			texldp r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, Tex0; w = 1, MaxMipLevel[0] = 5";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			mov r5, t1
			texldp r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, Tex0; w = 1, MaxMipLevel[0] = 6";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			mov r6, t1
			texldp r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex0; w = 1, MipMapLODBias[0] = 0";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			texldp r1, t1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex0; w = 1, MipMapLODBias[0] = 1";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			texldp r1, t1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex0; w = 1, MipMapLODBias[0] = 2";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			texldp r1, t1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex0; w = 1, MipMapLODBias[0] = 3";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			texldp r1, t1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex0; w = 1, MipMapLODBias[0] = 4";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			texldp r1, t1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex0; w = 1, MipMapLODBias[0] = 5";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			texldp r1, t1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex0; w = 1, MipMapLODBias[0] = 6";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			texldp r1, t1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = 1, MipMapLODBias[0] = 0";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			mov r0, t1
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, Tex0; w = 1, MipMapLODBias[0] = 1";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			mov r1, t1
			texldp r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, Tex0; w = 1, MipMapLODBias[0] = 2";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			mov r2, t1
			texldp r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, Tex0; w = 1, MipMapLODBias[0] = 3";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			mov r3, t1
			texldp r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, Tex0; w = 1, MipMapLODBias[0] = 4";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			mov r4, t1
			texldp r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, Tex0; w = 1, MipMapLODBias[0] = 5";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			mov r5, t1
			texldp r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, Tex0; w = 1, MipMapLODBias[0] = 6";
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
			ps_2_x
			dcl t1
            dcl_2d s0
			
			mov r6, t1
			texldp r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex0; w = 1/32, MipMapLODBias[0] = -0";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s0
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, Tex0; w = 1/32, MipMapLODBias[0] = -1";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s0
			
			mov r1, c0
			mov r1.x, t0.x
			mov r1.y, t0.y
			texldp r1, r1, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, Tex0; w = 1/32, MipMapLODBias[0] = -2";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s0
			
			mov r2, c0
			mov r2.x, t0.x
			mov r2.y, t0.y
			texldp r1, r2, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, Tex0; w = 1/32, MipMapLODBias[0] = -3";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s0
			
			mov r3, c0
			mov r3.x, t0.x
			mov r3.y, t0.y
			texldp r1, r3, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, Tex0; w = 1/32, MipMapLODBias[0] = -4";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s0
			
			mov r4, c0
			mov r4.x, t0.x
			mov r4.y, t0.y
			texldp r1, r4, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, Tex0; w = 1/32, MipMapLODBias[0] = -5";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s0
			
			mov r5, c0
			mov r5.x, t0.x
			mov r5.y, t0.y
			texldp r1, r5, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, Tex0; w = 1/32, MipMapLODBias[0] = -6";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s0
			
			mov r6, c0
			mov r6.x, t0.x
			mov r6.y, t0.y
			texldp r1, r6, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			texldp r0, t1, s9
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, Tex9";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			mov r1, t1
			texldp r0, r1, s9
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9; writemask = xyzw";
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
			ps_2_x
			dcl t1
			dcl_2d s9
			
			texldp r1.xyzw, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, Tex9; writemask = xyzw";
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
			ps_2_x
			dcl t1
			dcl_2d s9
			
			mov r1, t1
			texldp r1.xyzw, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : t1, Tex9";
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
			ps_2_x
			dcl t1
			dcl_2d s9
			
			texldp_pp r1, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : r1, Tex9";
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
			ps_2_x
			dcl t1
			dcl_2d s9
			
			mov r1, t1
			texldp_pp r1, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = 3.000000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 3.000000
            dcl_2d s9
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = 6.000000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 6.000000
            dcl_2d s9
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = -4.000000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -4.000000
            dcl_2d s9
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = -5.000000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -5.000000
            dcl_2d s9
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = 1/4.000000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 0.250000
            dcl_2d s9
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = 1/-6.000000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -0.166667
            dcl_2d s9
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = 1/1";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 1.000000
            dcl_2d s9
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = 1/2";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.500000
            dcl_2d s9
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = 1/4";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.250000
            dcl_2d s9
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = 1/8";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.125000
            dcl_2d s9
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = 1/16";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.062500
            dcl_2d s9
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = 1/32";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.031250
            dcl_2d s9
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = 1/64";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.015625
            dcl_2d s9
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9; w = 1, MaxMipLevel[9] = 0";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			texldp r1, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9; w = 1, MaxMipLevel[9] = 1";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			texldp r1, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9; w = 1, MaxMipLevel[9] = 2";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			texldp r1, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9; w = 1, MaxMipLevel[9] = 3";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			texldp r1, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9; w = 1, MaxMipLevel[9] = 4";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			texldp r1, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9; w = 1, MaxMipLevel[9] = 5";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			texldp r1, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9; w = 1, MaxMipLevel[9] = 6";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			texldp r1, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = 1, MaxMipLevel[9] = 0";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			mov r0, t1
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, Tex9; w = 1, MaxMipLevel[9] = 1";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			mov r1, t1
			texldp r1, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, Tex9; w = 1, MaxMipLevel[9] = 2";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			mov r2, t1
			texldp r1, r2, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, Tex9; w = 1, MaxMipLevel[9] = 3";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			mov r3, t1
			texldp r1, r3, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, Tex9; w = 1, MaxMipLevel[9] = 4";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			mov r4, t1
			texldp r1, r4, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, Tex9; w = 1, MaxMipLevel[9] = 5";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			mov r5, t1
			texldp r1, r5, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, Tex9; w = 1, MaxMipLevel[9] = 6";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			mov r6, t1
			texldp r1, r6, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9; w = 1, MipMapLODBias[9] = 0";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			texldp r1, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9; w = 1, MipMapLODBias[9] = 1";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			texldp r1, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9; w = 1, MipMapLODBias[9] = 2";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			texldp r1, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9; w = 1, MipMapLODBias[9] = 3";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			texldp r1, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9; w = 1, MipMapLODBias[9] = 4";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			texldp r1, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9; w = 1, MipMapLODBias[9] = 5";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			texldp r1, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex9; w = 1, MipMapLODBias[9] = 6";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			texldp r1, t1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = 1, MipMapLODBias[9] = 0";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			mov r0, t1
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, Tex9; w = 1, MipMapLODBias[9] = 1";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			mov r1, t1
			texldp r1, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, Tex9; w = 1, MipMapLODBias[9] = 2";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			mov r2, t1
			texldp r1, r2, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, Tex9; w = 1, MipMapLODBias[9] = 3";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			mov r3, t1
			texldp r1, r3, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, Tex9; w = 1, MipMapLODBias[9] = 4";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			mov r4, t1
			texldp r1, r4, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, Tex9; w = 1, MipMapLODBias[9] = 5";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			mov r5, t1
			texldp r1, r5, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, Tex9; w = 1, MipMapLODBias[9] = 6";
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
			ps_2_x
			dcl t1
            dcl_2d s9
			
			mov r6, t1
			texldp r1, r6, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex9; w = 1/32, MipMapLODBias[9] = -0";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s9
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, Tex9; w = 1/32, MipMapLODBias[9] = -1";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s9
			
			mov r1, c0
			mov r1.x, t0.x
			mov r1.y, t0.y
			texldp r1, r1, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, Tex9; w = 1/32, MipMapLODBias[9] = -2";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s9
			
			mov r2, c0
			mov r2.x, t0.x
			mov r2.y, t0.y
			texldp r1, r2, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, Tex9; w = 1/32, MipMapLODBias[9] = -3";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s9
			
			mov r3, c0
			mov r3.x, t0.x
			mov r3.y, t0.y
			texldp r1, r3, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, Tex9; w = 1/32, MipMapLODBias[9] = -4";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s9
			
			mov r4, c0
			mov r4.x, t0.x
			mov r4.y, t0.y
			texldp r1, r4, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, Tex9; w = 1/32, MipMapLODBias[9] = -5";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s9
			
			mov r5, c0
			mov r5.x, t0.x
			mov r5.y, t0.y
			texldp r1, r5, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, Tex9; w = 1/32, MipMapLODBias[9] = -6";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s9
			
			mov r6, c0
			mov r6.x, t0.x
			mov r6.y, t0.y
			texldp r1, r6, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t2, Tex14";
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
			ps_2_x
			dcl t2
            dcl_2d s14
			
			texldp r0, t2, s14
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, Tex14";
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
			ps_2_x
			dcl t2
            dcl_2d s14
			
			mov r2, t2
			texldp r0, r2, s14
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t2, Tex14; writemask = xyzw";
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
			ps_2_x
			dcl t2
			dcl_2d s14
			
			texldp r1.xyzw, t2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, Tex14; writemask = xyzw";
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
			ps_2_x
			dcl t2
			dcl_2d s14
			
			mov r2, t2
			texldp r1.xyzw, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : t2, Tex14";
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
			ps_2_x
			dcl t2
			dcl_2d s14
			
			texldp_pp r1, t2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp_pp : r2, Tex14";
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
			ps_2_x
			dcl t2
			dcl_2d s14
			
			mov r2, t2
			texldp_pp r1, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = 4.500000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 4.500000
            dcl_2d s14
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = 9.000000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 9.000000
            dcl_2d s14
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = -6.000000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -6.000000
            dcl_2d s14
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = -7.500000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -7.500000
            dcl_2d s14
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = 1/6.000000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, 0.166667
            dcl_2d s14
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = 1/-9.000000";
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
			ps_2_x
			dcl t0
			def  c0, 0.0, 0.0, 0.0, -0.111111
            dcl_2d s14
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = 1/1";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 1.000000
            dcl_2d s14
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = 1/2";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.500000
            dcl_2d s14
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = 1/4";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.250000
            dcl_2d s14
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = 1/8";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.125000
            dcl_2d s14
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = 1/16";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.062500
            dcl_2d s14
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = 1/32";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.031250
            dcl_2d s14
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = 1/64";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.015625
            dcl_2d s14
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex14; w = 1, MaxMipLevel[14] = 0";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			texldp r1, t1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex14; w = 1, MaxMipLevel[14] = 1";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			texldp r1, t1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex14; w = 1, MaxMipLevel[14] = 2";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			texldp r1, t1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex14; w = 1, MaxMipLevel[14] = 3";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			texldp r1, t1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex14; w = 1, MaxMipLevel[14] = 4";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			texldp r1, t1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex14; w = 1, MaxMipLevel[14] = 5";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			texldp r1, t1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex14; w = 1, MaxMipLevel[14] = 6";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			texldp r1, t1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = 1, MaxMipLevel[14] = 0";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			mov r0, t1
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, Tex14; w = 1, MaxMipLevel[14] = 1";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			mov r1, t1
			texldp r1, r1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, Tex14; w = 1, MaxMipLevel[14] = 2";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			mov r2, t1
			texldp r1, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, Tex14; w = 1, MaxMipLevel[14] = 3";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			mov r3, t1
			texldp r1, r3, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, Tex14; w = 1, MaxMipLevel[14] = 4";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			mov r4, t1
			texldp r1, r4, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, Tex14; w = 1, MaxMipLevel[14] = 5";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			mov r5, t1
			texldp r1, r5, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, Tex14; w = 1, MaxMipLevel[14] = 6";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			mov r6, t1
			texldp r1, r6, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex14; w = 1, MipMapLODBias[14] = 0";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			texldp r1, t1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex14; w = 1, MipMapLODBias[14] = 1";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			texldp r1, t1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex14; w = 1, MipMapLODBias[14] = 2";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			texldp r1, t1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex14; w = 1, MipMapLODBias[14] = 3";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			texldp r1, t1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex14; w = 1, MipMapLODBias[14] = 4";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			texldp r1, t1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex14; w = 1, MipMapLODBias[14] = 5";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			texldp r1, t1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : t1, Tex14; w = 1, MipMapLODBias[14] = 6";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			texldp r1, t1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = 1, MipMapLODBias[14] = 0";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			mov r0, t1
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, Tex14; w = 1, MipMapLODBias[14] = 1";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			mov r1, t1
			texldp r1, r1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, Tex14; w = 1, MipMapLODBias[14] = 2";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			mov r2, t1
			texldp r1, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, Tex14; w = 1, MipMapLODBias[14] = 3";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			mov r3, t1
			texldp r1, r3, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, Tex14; w = 1, MipMapLODBias[14] = 4";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			mov r4, t1
			texldp r1, r4, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, Tex14; w = 1, MipMapLODBias[14] = 5";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			mov r5, t1
			texldp r1, r5, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, Tex14; w = 1, MipMapLODBias[14] = 6";
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
			ps_2_x
			dcl t1
            dcl_2d s14
			
			mov r6, t1
			texldp r1, r6, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r0, Tex14; w = 1/32, MipMapLODBias[14] = -0";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s14
			
			mov r0, c0
			mov r0.x, t0.x
			mov r0.y, t0.y
			texldp r1, r0, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r1, Tex14; w = 1/32, MipMapLODBias[14] = -1";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s14
			
			mov r1, c0
			mov r1.x, t0.x
			mov r1.y, t0.y
			texldp r1, r1, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r2, Tex14; w = 1/32, MipMapLODBias[14] = -2";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s14
			
			mov r2, c0
			mov r2.x, t0.x
			mov r2.y, t0.y
			texldp r1, r2, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r3, Tex14; w = 1/32, MipMapLODBias[14] = -3";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s14
			
			mov r3, c0
			mov r3.x, t0.x
			mov r3.y, t0.y
			texldp r1, r3, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r4, Tex14; w = 1/32, MipMapLODBias[14] = -4";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s14
			
			mov r4, c0
			mov r4.x, t0.x
			mov r4.y, t0.y
			texldp r1, r4, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r5, Tex14; w = 1/32, MipMapLODBias[14] = -5";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s14
			
			mov r5, c0
			mov r5.x, t0.x
			mov r5.y, t0.y
			texldp r1, r5, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : r6, Tex14; w = 1/32, MipMapLODBias[14] = -6";
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
			ps_2_x
			dcl t0
            def  c0, 0.0, 0.0, 0.0, 0.03125
            dcl_2d s14
			
			mov r6, c0
			mov r6.x, t0.x
			mov r6.y, t0.y
			texldp r1, r6, s14
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
