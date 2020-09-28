#include "include.fx"
		
//cases with ifc
Technique 
< 
	String	Name = "ps_3_0 vFace : with if_lt, Back only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_lt vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with if_lt, Back only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_lt vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with if_lt, Back only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_lt vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with if_le, Back only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_le vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with if_le, Back only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_le vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with if_le, Back only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_le vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with if_gt, Back only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None; 
 		FillMode = Solid;
 		              
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_gt -vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with if_gt, Back only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None; 
 		FillMode = WireFrame;
 		              
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_gt -vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with if_gt, Back only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None; 
 		FillMode = Point;
 		              
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_gt -vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with if_ge, Back only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		          
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_ge -vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
			
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with if_ge, Back only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		          
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_ge -vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
			
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with if_ge, Back only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		          
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_ge -vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
			
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with if_gt, Front only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None; 
 		FillMode = Solid;
 		              
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_gt vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with if_gt, Front only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None; 
 		FillMode = WireFrame;
 		              
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_gt vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with if_gt, Front only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None; 
 		FillMode = Point;
 		              
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_gt vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with if_ge, Front only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		          
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_ge vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
			
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with if_ge, Front only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		          
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_ge vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
			
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with if_ge, Front only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		          
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_ge vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
			
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with if_lt, Front only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_lt -vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with if_lt, Front only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_lt -vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with if_lt, Front only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_lt -vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with if_le, Front only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_le -vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with if_le, Front only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_le -vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with if_le, Front only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			if_le -vFace, c0.x
				texld r0, v0, s0
			else
				mov r0, c0.z
			endif
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;
   }
}
//cases with setp
Technique
< 
	String	Name = "ps_3_0 vFace : with setp_lt, Back Only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_lt p0, vFace, c0.x
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 vFace : with setp_lt, Back Only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_lt p0, vFace, c0.x
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 vFace : with setp_lt, Back Only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_lt p0, vFace, c0.x
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 vFace : with setp_le, Back Only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_le p0, vFace, c0.x
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 vFace : with setp_le, Back Only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_le p0, vFace, c0.x
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 vFace : with setp_le, Back Only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_le p0, vFace, c0.x
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 -vFace : with setp_gt, Back Only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_gt p0, -vFace, c0.x
			
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 -vFace : with setp_gt, Back Only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_gt p0, -vFace, c0.x
			
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 -vFace : with setp_gt, Back Only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_gt p0, -vFace, c0.x
			
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 -vFace : with setp_ge, Back Only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_ge p0, -vFace, c0.x
			
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 -vFace : with setp_ge, Back Only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_ge p0, -vFace, c0.x
			
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 -vFace : with setp_ge, Back Only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_ge p0, -vFace, c0.x
			
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 vFace : with setp_gt, Front Only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_gt p0, vFace, c0.x
			
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 vFace : with setp_gt, Front Only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_gt p0, vFace, c0.x
			
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 vFace : with setp_gt, Front Only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_gt p0, vFace, c0.x
			
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 vFace : with setp_ge, Front Only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_ge p0, vFace, c0.x
			
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 vFace : with setp_ge, Front Only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_ge p0, vFace, c0.x
			
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 vFace : with setp_ge, Front Only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_ge p0, vFace, c0.x
			
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 -vFace : with setp_lt, Front Only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_lt p0, -vFace, c0.x
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 -vFace : with setp_lt, Front Only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_lt p0, -vFace, c0.x
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 -vFace : with setp_lt, Front Only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_lt p0, -vFace, c0.x
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 -vFace : with setp_le, Front Only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_le p0, -vFace, c0.x
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 -vFace : with setp_le, Front Only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_le p0, -vFace, c0.x
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
Technique
< 
	String	Name = "ps_3_0 -vFace : with setp_le, Front Only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0, 0, -1, 0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			setp_le p0, -vFace, c0.x
			( p0) texld r0, v0, s0
			(!p0) mov r0, c0.z
			
			texkill r0
			mov oC0, r0
        };
		VertexShader = <StandardVS>;	    
   }
}
//cases with cmp
Technique 
< 
	String	Name = "ps_3_0 vFace : with cmp, Back only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			texld r0, v0, s0
			cmp r1, vFace, c0.z, r0
			texkill r1
			mov oC0, r1
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with cmp, Back only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			texld r0, v0, s0
			cmp r1, vFace, c0.z, r0
			texkill r1
			mov oC0, r1
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with cmp, Back only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			texld r0, v0, s0
			cmp r1, vFace, c0.z, r0
			texkill r1
			mov oC0, r1
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with cmp, Back only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			texld r0, v0, s0
			cmp r1, -vFace, r0, c0.z
			texkill r1
			mov oC0, r1
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with cmp, Back only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			texld r0, v0, s0
			cmp r1, -vFace, r0, c0.z
			texkill r1
			mov oC0, r1
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with cmp, Back only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			texld r0, v0, s0
			cmp r1, -vFace, r0, c0.z
			texkill r1
			mov oC0, r1
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with cmp, Front only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Solid;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			texld r0, v0, s0
			cmp r1, vFace, r0, c0.z
			texkill r1
			mov oC0, r1
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with cmp, Front only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			texld r0, v0, s0
			cmp r1, vFace, r0, c0.z
			texkill r1
			mov oC0, r1
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vFace : with cmp, Front only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			texld r0, v0, s0
			cmp r1, vFace, r0, c0.z
			texkill r1
			mov oC0, r1
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with cmp, Front only";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			texld r0, v0, s0
			cmp r1, -vFace, c0.z, r0
			texkill r1
			mov oC0, r1
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with cmp, Front only, WireFrame FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = WireFrame;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			texld r0, v0, s0
			cmp r1, -vFace, c0.z, r0
			texkill r1
			mov oC0, r1
        };
		VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 -vFace : with cmp, Front only, Point FillMode";
	String	Shape = "Sphere";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_2>;
 		CullMode = None;
 		FillMode = Point;
 		
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			def c0, 0.0, 1.0, -1.0, 2.0
			dcl_texcoord0 v0
			dcl_2d s0
			dcl vFace
				
			texld r0, v0, s0
			cmp r1, -vFace, c0.z, r0
			texkill r1
			mov oC0, r1
        };
		VertexShader = <StandardVS>;
   }
}