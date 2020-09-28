#include "include.fx"

Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, level=0 => dsx=dsy=1/32";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceX, level=0 => dsx=dsy=1/32";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceY, level=0 => dsx=dsy=1/32";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, level=1 => dsx=dsy=2/32";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceX, level=1 => dsx=dsy=2/32";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceY, level=1 => dsx=dsy=2/32";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, level=2 => dsx=dsy=4/32";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceX, level=2 => dsx=dsy=4/32";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceY, level=2 => dsx=dsy=4/32";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, level=3 => dsx=dsy=8/32";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceX, level=3 => dsx=dsy=8/32";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceY, level=3 => dsx=dsy=8/32";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, level=4 => dsx=dsy=16/32";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceX, level=4 => dsx=dsy=16/32";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceY, level=4 => dsx=dsy=16/32";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, level=5 => dsx=dsy=32/32";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceX, level=5 => dsx=dsy=32/32";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceY, level=5 => dsx=dsy=32/32";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, x=1x, y=1y => level=0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceX, x=1x, y=1y => level=0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceY, x=1x, y=1y => level=0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, x=2x, y=2y => level=1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceX, x=2x, y=2y => level=1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceY, x=2x, y=2y => level=1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, x=4x, y=4y => level=2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceX, x=4x, y=4y => level=2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceY, x=4x, y=4y => level=2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, x=8x, y=8y => level=3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceX, x=8x, y=8y => level=3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceY, x=8x, y=8y => level=3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, x=16x, y=16y => level=4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceX, x=16x, y=16y => level=4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceY, x=16x, y=16y => level=4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, x=32x, y=32y => level=5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceX, x=32x, y=32y => level=5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceY, x=32x, y=32y => level=5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, repeat_by_x = 15";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 15.f, 15.f, 15.f, 15.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mul r0.x, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.xz, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2.xyzw, r0.xyzx
			dsy r3.xyzw, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4.xw, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4.xw, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4.yw, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4.yw, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceX, repeat_by_x = 15";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 15.f, 15.f, 15.f, 15.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mul r0.x, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			//mul r1.xz, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceY, repeat_by_x = 15";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 15.f, 15.f, 15.f, 15.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mul r0.x, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.yw, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, repeat_by_y = 15";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 15.f, 15.f, 15.f, 15.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mul r0.y, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.yw, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceX, repeat_by_y = 15";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 15.f, 15.f, 15.f, 15.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mul r0.y, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.yw, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceY, repeat_by_y = 15";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 15.f, 15.f, 15.f, 15.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mul r0.y, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			//mul r1.yw, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, repeat = 15";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 15.f, 15.f, 15.f, 15.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mul r0, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceX, repeat = 15";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 15.f, 15.f, 15.f, 15.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mul r0, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol4, SliceY, repeat = 15";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[4] = <TexVol_0>;
		MipFilter[4] = Point;
        MaxMipLevel[4] = 0;
        MipMapLODBias[4] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 15.f, 15.f, 15.f, 15.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s4
			
			mov r0.xyz, t0.xyz
			mul r0, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s4
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, level=0 => dsx=dsy=1/32";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceX, level=0 => dsx=dsy=1/32";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceY, level=0 => dsx=dsy=1/32";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, level=1 => dsx=dsy=2/32";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceX, level=1 => dsx=dsy=2/32";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceY, level=1 => dsx=dsy=2/32";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, level=2 => dsx=dsy=4/32";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceX, level=2 => dsx=dsy=4/32";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceY, level=2 => dsx=dsy=4/32";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, level=3 => dsx=dsy=8/32";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceX, level=3 => dsx=dsy=8/32";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceY, level=3 => dsx=dsy=8/32";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, level=4 => dsx=dsy=16/32";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceX, level=4 => dsx=dsy=16/32";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceY, level=4 => dsx=dsy=16/32";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, level=5 => dsx=dsy=32/32";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceX, level=5 => dsx=dsy=32/32";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceY, level=5 => dsx=dsy=32/32";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, x=1x, y=1y => level=0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceX, x=1x, y=1y => level=0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceY, x=1x, y=1y => level=0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, x=2x, y=2y => level=1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceX, x=2x, y=2y => level=1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceY, x=2x, y=2y => level=1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, x=4x, y=4y => level=2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceX, x=4x, y=4y => level=2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceY, x=4x, y=4y => level=2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, x=8x, y=8y => level=3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceX, x=8x, y=8y => level=3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceY, x=8x, y=8y => level=3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, x=16x, y=16y => level=4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceX, x=16x, y=16y => level=4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceY, x=16x, y=16y => level=4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, x=32x, y=32y => level=5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceX, x=32x, y=32y => level=5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceY, x=32x, y=32y => level=5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, repeat_by_x = 150";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 150.f, 150.f, 150.f, 150.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mul r0.x, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.xz, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2.xyzw, r0.xyzx
			dsy r3.xyzw, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4.xw, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4.xw, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4.yw, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4.yw, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceX, repeat_by_x = 150";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 150.f, 150.f, 150.f, 150.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mul r0.x, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			//mul r1.xz, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceY, repeat_by_x = 150";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 150.f, 150.f, 150.f, 150.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mul r0.x, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.yw, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, repeat_by_y = 150";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 150.f, 150.f, 150.f, 150.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mul r0.y, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.yw, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceX, repeat_by_y = 150";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 150.f, 150.f, 150.f, 150.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mul r0.y, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.yw, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceY, repeat_by_y = 150";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 150.f, 150.f, 150.f, 150.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mul r0.y, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			//mul r1.yw, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, repeat = 150";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 150.f, 150.f, 150.f, 150.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mul r0, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceX, repeat = 150";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 150.f, 150.f, 150.f, 150.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mul r0, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol8, SliceY, repeat = 150";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
 		
 		PixelShaderConstant[1] = <fVolSizes>;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 150.f, 150.f, 150.f, 150.f
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s8
			
			mov r0.xyz, t0.xyz
			mul r0, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s8
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, level=0 => dsx=dsy=1/32";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceX, level=0 => dsx=dsy=1/32";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceY, level=0 => dsx=dsy=1/32";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, level=1 => dsx=dsy=2/32";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceX, level=1 => dsx=dsy=2/32";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceY, level=1 => dsx=dsy=2/32";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, level=2 => dsx=dsy=4/32";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceX, level=2 => dsx=dsy=4/32";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceY, level=2 => dsx=dsy=4/32";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, level=3 => dsx=dsy=8/32";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceX, level=3 => dsx=dsy=8/32";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceY, level=3 => dsx=dsy=8/32";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, level=4 => dsx=dsy=16/32";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceX, level=4 => dsx=dsy=16/32";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceY, level=4 => dsx=dsy=16/32";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, level=5 => dsx=dsy=32/32";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceX, level=5 => dsx=dsy=32/32";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceY, level=5 => dsx=dsy=32/32";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, x=1x, y=1y => level=0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceX, x=1x, y=1y => level=0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceY, x=1x, y=1y => level=0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 1.0, 1.0, 1.0, 1.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, x=2x, y=2y => level=1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceX, x=2x, y=2y => level=1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceY, x=2x, y=2y => level=1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 2.0, 2.0, 2.0, 2.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, x=4x, y=4y => level=2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceX, x=4x, y=4y => level=2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceY, x=4x, y=4y => level=2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 4.0, 4.0, 4.0, 4.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, x=8x, y=8y => level=3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceX, x=8x, y=8y => level=3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceY, x=8x, y=8y => level=3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 8.0, 8.0, 8.0, 8.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, x=16x, y=16y => level=4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceX, x=16x, y=16y => level=4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceY, x=16x, y=16y => level=4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 16.0, 16.0, 16.0, 16.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, x=32x, y=32y => level=5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.xy, t0.xy, c1.xy
			mov r0.z, t0.z
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xy, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceX, x=32x, y=32y => level=5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.yz, t0.xyz, c1.yyx
			mov r0.x, t0.x
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.yz, r0, r1
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceY, x=32x, y=32y => level=5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			def  c0, 32.0, 32.0, 32.0, 32.0
            def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mul r0.xz, t0.xyz, c1.yyx
			mov r0.y, t0.y
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0, r1
			mul r3, r1, c2.w
			mul r0.xz, r0, r1.yyz
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3 
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, repeat_by_x = MaxTexRepeat";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[0] = <fMaxTexRepeat>;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mul r0.x, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.xz, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2.xyzw, r0.xyzx
			dsy r3.xyzw, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4.xw, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4.xw, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4.yw, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4.yw, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceX, repeat_by_x = MaxTexRepeat";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[0] = <fMaxTexRepeat>;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mul r0.x, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			//mul r1.xz, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceY, repeat_by_x = MaxTexRepeat";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[0] = <fMaxTexRepeat>;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mul r0.x, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.yw, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, repeat_by_y = MaxTexRepeat";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[0] = <fMaxTexRepeat>;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mul r0.y, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.yw, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceX, repeat_by_y = MaxTexRepeat";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[0] = <fMaxTexRepeat>;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mul r0.y, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1.yw, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceY, repeat_by_y = MaxTexRepeat";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[0] = <fMaxTexRepeat>;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mul r0.y, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			//mul r1.yw, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, repeat = MaxTexRepeat";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[0] = <fMaxTexRepeat>;
 		PixelShaderConstant[1] = <fVolSizes>;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mul r0, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzx
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (x1,0,0,x1)
			mov r4, r2
			add r4.xw, r4, r1.x
			add r4.yz, r4.yz, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (x2,0,0,x2)
			mov r4, r2
			add r4.xw, r4, r1.z
			add r4.yz, r4.yz, -r5.x
			add r4.yz, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceX, repeat = MaxTexRepeat";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[0] = <fMaxTexRepeat>;
 		PixelShaderConstant[1] = <fVolSizes>;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mul r0, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzy
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (0,y1,0,y1)
			mov r4, r3
			add r4.yw, r4, r1.y
			add r4.xz, r4.xz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (0,y2,0,y2)
			mov r4, r3
			add r4.yw, r4.yw, r1.w
			add r4.xz, r4.xz, -r5.y
			add r4.xz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_x dsx_dsy : TexVol13, SliceY, repeat = MaxTexRepeat";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
 		PixelShaderConstant[0] = <fMaxTexRepeat>;
 		PixelShaderConstant[1] = <fVolSizes>;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			dcl t0.xyz
			
			def  c2, -1.0, 0.0, 0.0, 0.015
            dcl_volume s13
			
			mov r0.xyz, t0.xyz
			mul r0, r0, c0.x
			
			mov r1, c1.zwzw
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mul r1, c0.x, r1
			mul r3, r1, c2.w
			add r1.xy, r1.xy, -r3.xy 
			add r1.zw, r1, r3
			mov r5, r3
			dsx r2, r0.xyzz
			dsy r3, r0.xyzx
			
			texld r0, r0, s13
			mul r1, r1, c2.x
			add r5.zw, r5.xy, r5.xy
			add r5.zw, r5.zw, r5.xy
			mul r5, r5, c2.x
			
			//make sure that dsx >= (0,0,x1,x1)
			mov r4, r2
			add r4.zw, r4, r1.x
			add r4.xy, r4.xy, -r5.x
			cmp r0, r4, r0, c2.x
			//make sure that dsx <= (0,0,x2,x2)
			mov r4, r2
			add r4.zw, r4, r1.z
			add r4.xy, r4.xy, -r5.x
			add r4.xy, r4, r5.z
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			//make sure that dsy >= (y1,0,0,y1)
			mov r4, r3
			add r4.xw, r4, r1.y
			add r4.yz, r4.yz, -r5.y
			cmp r0, r4, r0, c2.x
			//make sure that dsy <= (y2,0,0,y2)
			mov r4, r3
			add r4.xw, r4.yw, r1.w
			add r4.yz, r4.yz, -r5.y
			add r4.yz, r4.xz, r5.w
			mul r4, r4, c2.x
			cmp r0, r4, r0, c2.x
			texkill r0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
