#include "include.fx"

Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1.xyzw, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 2.000000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.000000, 2.000000, 2.000000, 2.000000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 2.000000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.000000, 2.000000, 2.000000, 2.000000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 2.000000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.000000, 2.000000, 2.000000, 2.000000
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 3.500000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 3.500000, 3.500000, 3.500000, 3.500000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 3.500000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 3.500000, 3.500000, 3.500000, 3.500000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 3.500000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 3.500000, 3.500000, 3.500000, 3.500000
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = -1.500000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -1.500000, -1.500000, -1.500000, -1.500000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = -1.500000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -1.500000, -1.500000, -1.500000, -1.500000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = -1.500000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -1.500000, -1.500000, -1.500000, -1.500000
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = -3.000000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -3.000000, -3.000000, -3.000000, -3.000000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = -3.000000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -3.000000, -3.000000, -3.000000, -3.000000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = -3.000000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -3.000000, -3.000000, -3.000000, -3.000000
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/2.500000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 0.400000, 0.400000, 0.400000, 0.400000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/2.500000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 0.400000, 0.400000, 0.400000, 0.400000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/2.500000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 0.400000, 0.400000, 0.400000, 0.400000
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/-4.000000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -0.250000, -0.250000, -0.250000, -0.250000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/-4.000000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -0.250000, -0.250000, -0.250000, -0.250000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/-4.000000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -0.250000, -0.250000, -0.250000, -0.250000
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/1";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/1";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/1";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/2";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/2";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/2";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/4";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/4";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/4";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/8";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/8";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/8";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/16";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/16";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/16";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/32";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/32";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/32";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/64";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/64";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/64";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/128";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.007813, 0.007813, 0.007813, 0.007813
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/128";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.007813, 0.007813, 0.007813, 0.007813
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/128";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.007813, 0.007813, 0.007813, 0.007813
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MaxMipLevel[1] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MaxMipLevel[1] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 1;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MaxMipLevel[1] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 2;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MaxMipLevel[1] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 3;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MaxMipLevel[1] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 4;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MaxMipLevel[1] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 5;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MaxMipLevel[1] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 6;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MaxMipLevel[1] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 7;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MipMapLODBias[1] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MipMapLODBias[1] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MipMapLODBias[1] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MipMapLODBias[1] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MipMapLODBias[1] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MipMapLODBias[1] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MipMapLODBias[1] = 6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, w = 1, MipMapLODBias[1] = 7";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = 7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s1
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/64, MipMapLODBias[1] = -0";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/64, MipMapLODBias[1] = -0";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/64, MipMapLODBias[1] = -0";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/64, MipMapLODBias[1] = -1";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/64, MipMapLODBias[1] = -1";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/64, MipMapLODBias[1] = -1";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/64, MipMapLODBias[1] = -2";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/64, MipMapLODBias[1] = -2";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/64, MipMapLODBias[1] = -2";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/64, MipMapLODBias[1] = -3";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/64, MipMapLODBias[1] = -3";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/64, MipMapLODBias[1] = -3";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/64, MipMapLODBias[1] = -4";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/64, MipMapLODBias[1] = -4";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/64, MipMapLODBias[1] = -4";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/64, MipMapLODBias[1] = -5";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/64, MipMapLODBias[1] = -5";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/64, MipMapLODBias[1] = -5";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/64, MipMapLODBias[1] = -6";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/64, MipMapLODBias[1] = -6";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/64, MipMapLODBias[1] = -6";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube1, Faces01, w = 1/64, MipMapLODBias[1] = -7";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces23, w = 1/64, MipMapLODBias[1] = -7";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s1
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube1, Faces45, w = 1/64, MipMapLODBias[1] = -7";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[1] = <TexCube_0>;
 		MipFilter[1] = Point;
        MaxMipLevel[1] = 0;
        MipMapLODBias[1] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s1
			
			mov r0, t0.xyz
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
	String	Name = "ps_2_x texldp : TexCube8, w = 1, writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1.xyzw, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 4.000000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.000000, 4.000000, 4.000000, 4.000000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 4.000000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.000000, 4.000000, 4.000000, 4.000000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 4.000000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.000000, 4.000000, 4.000000, 4.000000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 7.000000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 7.000000, 7.000000, 7.000000, 7.000000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 7.000000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 7.000000, 7.000000, 7.000000, 7.000000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 7.000000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 7.000000, 7.000000, 7.000000, 7.000000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = -3.000000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -3.000000, -3.000000, -3.000000, -3.000000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = -3.000000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -3.000000, -3.000000, -3.000000, -3.000000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = -3.000000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -3.000000, -3.000000, -3.000000, -3.000000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = -6.000000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -6.000000, -6.000000, -6.000000, -6.000000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = -6.000000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -6.000000, -6.000000, -6.000000, -6.000000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = -6.000000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -6.000000, -6.000000, -6.000000, -6.000000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/5.000000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 0.200000, 0.200000, 0.200000, 0.200000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/5.000000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 0.200000, 0.200000, 0.200000, 0.200000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/5.000000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 0.200000, 0.200000, 0.200000, 0.200000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/-8.000000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -0.125000, -0.125000, -0.125000, -0.125000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/-8.000000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -0.125000, -0.125000, -0.125000, -0.125000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/-8.000000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -0.125000, -0.125000, -0.125000, -0.125000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/1";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/1";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/1";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/2";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/2";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/2";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/4";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/4";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/4";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/8";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/8";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/8";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/16";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/16";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/16";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/32";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/32";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/32";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/64";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/64";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/64";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/128";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.007813, 0.007813, 0.007813, 0.007813
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/128";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.007813, 0.007813, 0.007813, 0.007813
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/128";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.007813, 0.007813, 0.007813, 0.007813
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MaxMipLevel[8] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MaxMipLevel[8] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 1;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MaxMipLevel[8] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MaxMipLevel[8] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 3;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MaxMipLevel[8] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 4;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MaxMipLevel[8] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 5;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MaxMipLevel[8] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 6;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MaxMipLevel[8] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 7;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MipMapLODBias[8] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MipMapLODBias[8] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MipMapLODBias[8] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MipMapLODBias[8] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MipMapLODBias[8] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MipMapLODBias[8] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MipMapLODBias[8] = 6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, w = 1, MipMapLODBias[8] = 7";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s8
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/64, MipMapLODBias[8] = -0";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/64, MipMapLODBias[8] = -0";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/64, MipMapLODBias[8] = -0";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/64, MipMapLODBias[8] = -1";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/64, MipMapLODBias[8] = -1";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/64, MipMapLODBias[8] = -1";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/64, MipMapLODBias[8] = -2";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/64, MipMapLODBias[8] = -2";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/64, MipMapLODBias[8] = -2";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/64, MipMapLODBias[8] = -3";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/64, MipMapLODBias[8] = -3";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/64, MipMapLODBias[8] = -3";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/64, MipMapLODBias[8] = -4";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/64, MipMapLODBias[8] = -4";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/64, MipMapLODBias[8] = -4";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/64, MipMapLODBias[8] = -5";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/64, MipMapLODBias[8] = -5";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/64, MipMapLODBias[8] = -5";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/64, MipMapLODBias[8] = -6";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/64, MipMapLODBias[8] = -6";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/64, MipMapLODBias[8] = -6";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces01, w = 1/64, MipMapLODBias[8] = -7";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces23, w = 1/64, MipMapLODBias[8] = -7";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube8, Faces45, w = 1/64, MipMapLODBias[8] = -7";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[8] = <TexCube_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s8
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s8
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, writemask=xyzw";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1.xyzw, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 6.000000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 6.000000, 6.000000, 6.000000, 6.000000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 6.000000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 6.000000, 6.000000, 6.000000, 6.000000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 6.000000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 6.000000, 6.000000, 6.000000, 6.000000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 10.500000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 10.500000, 10.500000, 10.500000, 10.500000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 10.500000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 10.500000, 10.500000, 10.500000, 10.500000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 10.500000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 10.500000, 10.500000, 10.500000, 10.500000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = -4.500000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -4.500000, -4.500000, -4.500000, -4.500000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = -4.500000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -4.500000, -4.500000, -4.500000, -4.500000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = -4.500000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -4.500000, -4.500000, -4.500000, -4.500000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = -9.000000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -9.000000, -9.000000, -9.000000, -9.000000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = -9.000000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -9.000000, -9.000000, -9.000000, -9.000000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = -9.000000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -9.000000, -9.000000, -9.000000, -9.000000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/7.500000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 0.133333, 0.133333, 0.133333, 0.133333
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/7.500000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 0.133333, 0.133333, 0.133333, 0.133333
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/7.500000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 0.133333, 0.133333, 0.133333, 0.133333
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/-12.000000";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -0.083333, -0.083333, -0.083333, -0.083333
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/-12.000000";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -0.083333, -0.083333, -0.083333, -0.083333
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/-12.000000";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = None;
 		CullMode = None;           
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, -0.083333, -0.083333, -0.083333, -0.083333
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
			
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/1";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/1";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/1";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/2";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/2";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/2";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/4";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/4";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/4";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/8";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/8";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/8";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/16";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/16";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/16";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/32";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/32";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/32";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/64";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/64";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/64";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/128";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.007813, 0.007813, 0.007813, 0.007813
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/128";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.007813, 0.007813, 0.007813, 0.007813
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/128";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;         
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.007813, 0.007813, 0.007813, 0.007813
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MaxMipLevel[13] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MaxMipLevel[13] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MaxMipLevel[13] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 2;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MaxMipLevel[13] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 3;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MaxMipLevel[13] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 4;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MaxMipLevel[13] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 5;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MaxMipLevel[13] = 6";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 6;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MaxMipLevel[13] = 7";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 7;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MipMapLODBias[13] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MipMapLODBias[13] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MipMapLODBias[13] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MipMapLODBias[13] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MipMapLODBias[13] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MipMapLODBias[13] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MipMapLODBias[13] = 6";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, w = 1, MipMapLODBias[13] = 7";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			texldp r1, r0, s13
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/64, MipMapLODBias[13] = -0";
	String	Shape = "Faces01";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/64, MipMapLODBias[13] = -0";
	String	Shape = "Faces23";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/64, MipMapLODBias[13] = -0";
	String	Shape = "Faces45";
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/64, MipMapLODBias[13] = -1";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/64, MipMapLODBias[13] = -1";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/64, MipMapLODBias[13] = -1";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/64, MipMapLODBias[13] = -2";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/64, MipMapLODBias[13] = -2";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/64, MipMapLODBias[13] = -2";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/64, MipMapLODBias[13] = -3";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/64, MipMapLODBias[13] = -3";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/64, MipMapLODBias[13] = -3";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/64, MipMapLODBias[13] = -4";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/64, MipMapLODBias[13] = -4";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/64, MipMapLODBias[13] = -4";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/64, MipMapLODBias[13] = -5";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/64, MipMapLODBias[13] = -5";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/64, MipMapLODBias[13] = -5";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/64, MipMapLODBias[13] = -6";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/64, MipMapLODBias[13] = -6";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/64, MipMapLODBias[13] = -6";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces01, w = 1/64, MipMapLODBias[13] = -7";
	String	Shape = "Faces01";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.x, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces23, w = 1/64, MipMapLODBias[13] = -7";
	String	Shape = "Faces23";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.y, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x texldp : TexCube13, Faces45, w = 1/64, MipMapLODBias[13] = -7";
	String	Shape = "Faces45";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[13] = <TexCube_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -7;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_cube s13
			
			mov r0, t0.xyz
			mov r0.w, c0.x
			mul r0.z, r0, c0
			texldp r1, r0, s13
			mov oC0, r1
        };
		VertexShader = <VolTLVS>;
   }
}
