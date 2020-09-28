#include "include.fx"

Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex0, level=0 => dsx=dsy=1/32";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s0
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s0
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex0, level=1 => dsx=dsy=2/32";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s0
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s0
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex0, level=2 => dsx=dsy=4/32";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s0
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s0
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex0, level=3 => dsx=dsy=8/32";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s0
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s0
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex0, level=4 => dsx=dsy=16/32";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s0
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s0
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex0, level=5 => dsx=dsy=32/32";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s0
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s0
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex0, x=1x, y=1y => level=0";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s0
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s0
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex0, x=2x, y=2y => level=1";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s0
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s0
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex0, x=4x, y=4y => level=2";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s0
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s0
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex0, x=8x, y=8y => level=3";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s0
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s0
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex0, x=16x, y=16y => level=4";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s0
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s0
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex0, x=32x, y=32y => level=5";
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
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s0
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s0
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex0, repeat_by_x = 15";
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
 		
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 15.f, 15.f, 15.f, 15.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s0
			
			mov r0.xy, v0.xy
			mul r0.x, r0, c0.x
			
			mov r1, c1.xyxy
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.xz, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1.xyzw, r3.xyzw
			mov r5, r3
			dsx r2, r0.xyxx
			dsy r3, r0.xyyy
			
			texld r0, r0, s0
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4.xzw, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4.xzw, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4.yzw, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4.yzw, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex0, repeat_by_y = 15";
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
 		
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 15.f, 15.f, 15.f, 15.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s0
			
			mov r0.xy, v0.xy
			mul r0.y, r0, c0.x
			
			mov r1, c1.xyxy
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.yw, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1.xyzw, r3.xyzw
			mov r5, r3
			dsx r2, r0.xyxx
			dsy r3, r0.xyyy
			
			texld r0, r0, s0
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex0, repeat = 15";
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
 		
 		PixelShaderConstant[1] = <fTexSizes>;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 15.f, 15.f, 15.f, 15.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s0
			
			mov r0.xy, v0.xy
			mul r0, r0, c0.x
			
			mov r1, c1.xyxy
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1.xyzw, r3.xyzw
			mov r5, r3
			dsx r2, r0.xyxx
			dsy r3, r0.xyyy
			
			texld r0, r0, s0
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex6, level=0 => dsx=dsy=1/32";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s6
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s6
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex6, level=1 => dsx=dsy=2/32";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s6
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s6
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex6, level=2 => dsx=dsy=4/32";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s6
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s6
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex6, level=3 => dsx=dsy=8/32";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s6
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s6
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex6, level=4 => dsx=dsy=16/32";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s6
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s6
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex6, level=5 => dsx=dsy=32/32";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s6
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s6
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex6, x=1x, y=1y => level=0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s6
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s6
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex6, x=2x, y=2y => level=1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s6
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s6
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex6, x=4x, y=4y => level=2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s6
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s6
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex6, x=8x, y=8y => level=3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s6
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s6
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex6, x=16x, y=16y => level=4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s6
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s6
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex6, x=32x, y=32y => level=5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s6
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s6
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex6, repeat_by_x = 150";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 150.f, 150.f, 150.f, 150.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s6
			
			mov r0.xy, v0.xy
			mul r0.x, r0, c0.x
			
			mov r1, c1.xyxy
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.xz, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1.xyzw, r3.xyzw
			mov r5, r3
			dsx r2, r0.xyxx
			dsy r3, r0.xyyy
			
			texld r0, r0, s6
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4.xzw, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4.xzw, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4.yzw, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4.yzw, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex6, repeat_by_y = 150";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 150.f, 150.f, 150.f, 150.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s6
			
			mov r0.xy, v0.xy
			mul r0.y, r0, c0.x
			
			mov r1, c1.xyxy
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.yw, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1.xyzw, r3.xyzw
			mov r5, r3
			dsx r2, r0.xyxx
			dsy r3, r0.xyyy
			
			texld r0, r0, s6
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex6, repeat = 150";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
		MipFilter[6] = Point;
        MaxMipLevel[6] = 0;
        MipMapLODBias[6] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fTexSizes>;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 150.f, 150.f, 150.f, 150.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s6
			
			mov r0.xy, v0.xy
			mul r0, r0, c0.x
			
			mov r1, c1.xyxy
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1.xyzw, r3.xyzw
			mov r5, r3
			dsx r2, r0.xyxx
			dsy r3, r0.xyyy
			
			texld r0, r0, s6
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex11, level=0 => dsx=dsy=1/32";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s11
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s11
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex11, level=1 => dsx=dsy=2/32";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s11
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s11
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex11, level=2 => dsx=dsy=4/32";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s11
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s11
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex11, level=3 => dsx=dsy=8/32";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s11
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s11
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex11, level=4 => dsx=dsy=16/32";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s11
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s11
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex11, level=5 => dsx=dsy=32/32";
	String	Shape = "TexTL5";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s11
			
			mov r0.xy, v0.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s11
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex11, x=1x, y=1y => level=0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s11
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s11
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex11, x=2x, y=2y => level=1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s11
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s11
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex11, x=4x, y=4y => level=2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s11
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s11
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex11, x=8x, y=8y => level=3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s11
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s11
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex11, x=16x, y=16y => level=4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s11
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s11
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex11, x=32x, y=32y => level=5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s11
			
			mul r0.xy, v0.xy, c1.xy
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0.xy, r1.xy
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1.xyzw, r3.xyzw 
			mov r5, r3
			dsx r2.xyzw, r0.xyxx
			dsy r3.xyzw, r0.xyyy
			
			texld r0, r0, s11
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex11, repeat_by_x = MaxTexRepeat";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[0] = <fMaxTexRepeat>;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s11
			
			mov r0.xy, v0.xy
			mul r0.x, r0, c0.x
			
			mov r1, c1.xyxy
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.xz, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1.xyzw, r3.xyzw
			mov r5, r3
			dsx r2, r0.xyxx
			dsy r3, r0.xyyy
			
			texld r0, r0, s11
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4.xzw, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4.xzw, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4.yzw, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4.yzw, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex11, repeat_by_y = MaxTexRepeat";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[0] = <fMaxTexRepeat>;
 		PixelShaderConstant[1] = <fTexSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s11
			
			mov r0.xy, v0.xy
			mul r0.y, r0, c0.x
			
			mov r1, c1.xyxy
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.yw, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1.xyzw, r3.xyzw
			mov r5, r3
			dsx r2, r0.xyxx
			dsy r3, r0.xyyy
			
			texld r0, r0, s11
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 dsx_dsy : Tex11, repeat = MaxTexRepeat";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
		MipFilter[11] = Point;
        MaxMipLevel[11] = 0;
        MipMapLODBias[11] = 0;
 		CullMode = None;
 		PixelShaderConstant[0] = <fMaxTexRepeat>;
 		PixelShaderConstant[1] = <fTexSizes>;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xy
			
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_2d s11
			
			mov r0.xy, v0.xy
			mul r0, r0, c0.x
			
			mov r1, c1.xyxy
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1.xyzw, r3.xyzw
			mov r5, r3
			dsx r2, r0.xyxx
			dsy r3, r0.xyyy
			
			texld r0, r0, s11
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r2.y, r2.y, r5.z
			add r3.x, r3.x, r5.w
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,x1,x1)
			mov r4, r2
			add r4.xzw, r4, r1.x
			add r4.y, r4.y, r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,x2,x2)
			mov r4, r2
			add r4.xzw, r4, r1.z
			add r4.y, r4.y, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,y1,y1)
			mov r4, r3
			add r4.yzw, r4, r1.y
			add r4.x, r4.x, r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,y2,y2)
			mov r4, r3
			add r4.yzw, r4, r1.w
			add r4.x, r4.x, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}