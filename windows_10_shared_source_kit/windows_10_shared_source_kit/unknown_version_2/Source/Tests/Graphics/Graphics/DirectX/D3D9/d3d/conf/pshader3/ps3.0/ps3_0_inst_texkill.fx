#include "include.fx"
		
Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, Full WriteMask, Any Component < 0";
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
			def c0, -0.2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_2d s0
				
			texld r0, v0, s0
			add r0, r0, c0.x
			texkill r0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill,  Full WriteMask, .r < 0";
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
			def c0, -0.2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_2d s0
				
			texld r0, v0, s0
			add r0.r, r0.r, c0.r
			texkill r0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, Full WriteMask, .g < 0";
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
			def c0, -0.2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_2d s0
				
			texld r0, v0, s0
			add r0.g, r0.g, c0.r
			texkill r0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, Full WriteMask, .b < 0";
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
			def c0, -0.2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_2d s0
				
			texld r0, v0, s0
			add r0.b, r0.b, c0.r
			texkill r0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, Full WriteMask, .a < 0";
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
			def c0, -0.2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_2d s0
				
			texld r0, v0, s0
			add r0.a, r0.a, c0.r
			texkill r0
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill,  .r WriteMask, .r < 0";
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
			def c0, -0.2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_2d s0
				
			texld r0, v0, s0
			add r0.r, r0.r, c0.r
			texkill r0.r
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, .g WriteMask, .g < 0";
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
			def c0, -0.2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_2d s0
				
			texld r0, v0, s0
			add r0.g, r0.g, c0.r
			texkill r0.g
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, .b WriteMask, .b < 0";
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
			def c0, -0.2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_2d s0
				
			texld r0, v0, s0
			add r0.b, r0.b, c0.r
			texkill r0.b
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, .a WriteMask, .a < 0";
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
			def c0, -0.2, 0, 0, 0
			dcl_texcoord0 v0
			dcl_2d s0
				
			texld r0, v0, s0
			add r0.a, r0.a, c0.r
			texkill r0.a
			mov oC0, r0
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "texkill : Tex0, writemask=a";
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
			def c0, -0.2, -0.2, -0.2, -0.2
			dcl_texcoord0 v0.xy
            dcl_2d s0
			
			texld r0, v0, s0
			add r0, r0, c0
			texkill r0.a
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texkill : Tex1, writemask=g";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[1] = <Tex2D_0>;
 		MipFilter[1] = None;  
 		CullMode = None;  
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def c0, -0.2, -0.2, -0.2, -0.2
			dcl_texcoord0 v0.xy
            dcl_2d s1
			
			texld r0, v0, s1
			add r0, r0, c0
			texkill r0.g
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texkill : Tex2, writemask=b";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[2] = <Tex2D_0>;
 		MipFilter[2] = None;  
 		CullMode = None;  
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def c0, -0.2, -0.2, -0.2, -0.2
			dcl_texcoord0 v0.xy
            dcl_2d s2
			
			texld r0, v0, s2
			add r0, r0, c0
			texkill r0.b
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texkill : Tex3, writemask=a";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[3] = <Tex2D_0>;
 		MipFilter[3] = None;  
 		CullMode = None;  
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def c0, -0.2, -0.2, -0.2, -0.2
			dcl_texcoord0 v0.xy
            dcl_2d s3
			
			texld r0, v0, s3
			add r0, r0, c0
			texkill r0.a
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texkill : Tex4, writemask=rg";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[4] = <Tex2D_0>;
 		MipFilter[4] = None;  
 		CullMode = None;  
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def c0, -0.2, -0.2, -0.2, -0.2
			dcl_texcoord0 v0.xy
            dcl_2d s4
			
			texld r0, v0, s4
			add r0, r0, c0
			texkill r0.rg
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texkill : Tex5, writemask=rb";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[5] = <Tex2D_0>;
 		MipFilter[5] = None;  
 		CullMode = None;  
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def c0, -0.2, -0.2, -0.2, -0.2
			dcl_texcoord0 v0.xy
            dcl_2d s5
			
			texld r0, v0, s5
			add r0, r0, c0
			texkill r0.rb
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texkill : Tex6, writemask=ra";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[6] = <Tex2D_0>;
 		MipFilter[6] = None;  
 		CullMode = None;  
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def c0, -0.2, -0.2, -0.2, -0.2
			dcl_texcoord0 v0.xy
            dcl_2d s6
			
			texld r0, v0, s6
			add r0, r0, c0
			texkill r0.ra
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texkill : Tex7, writemask=gb";
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
			def c0, -0.2, -0.2, -0.2, -0.2
			dcl_texcoord0 v0.xy
            dcl_2d s7
			
			texld r0, v0, s7
			add r0, r0, c0
			texkill r0.gb
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texkill : Tex8, writemask=ga";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[8] = <Tex2D_0>;
 		MipFilter[8] = None;  
 		CullMode = None;  
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def c0, -0.2, -0.2, -0.2, -0.2
			dcl_texcoord0 v0.xy
            dcl_2d s8
			
			texld r0, v0, s8
			add r0, r0, c0
			texkill r0.ga
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texkill : Tex9, writemask=ba";
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
			ps_3_0
			def c0, -0.2, -0.2, -0.2, -0.2
			dcl_texcoord0 v0.xy
            dcl_2d s9
			
			texld r0, v0, s9
			add r0, r0, c0
			texkill r0.ba
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texkill : Tex10, writemask=rgb";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[10] = <Tex2D_0>;
 		MipFilter[10] = None;  
 		CullMode = None;  
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def c0, -0.2, -0.2, -0.2, -0.2
			dcl_texcoord0 v0.xy
            dcl_2d s10
			
			texld r0, v0, s10
			add r0, r0, c0
			texkill r0.rgb
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texkill : Tex11, writemask=rga";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[11] = <Tex2D_0>;
 		MipFilter[11] = None;  
 		CullMode = None;  
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def c0, -0.2, -0.2, -0.2, -0.2
			dcl_texcoord0 v0.xy
            dcl_2d s11
			
			texld r0, v0, s11
			add r0, r0, c0
			texkill r0.rga
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texkill : Tex12, writemask=rba";
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
			def c0, -0.2, -0.2, -0.2, -0.2
			dcl_texcoord0 v0.xy
            dcl_2d s12
			
			texld r0, v0, s12
			add r0, r0, c0
			texkill r0.rba
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texkill : Tex13, writemask=gba";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[13] = <Tex2D_0>;
 		MipFilter[13] = None;  
 		CullMode = None;  
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def c0, -0.2, -0.2, -0.2, -0.2
			dcl_texcoord0 v0.xy
            dcl_2d s13
			
			texld r0, v0, s13
			add r0, r0, c0
			texkill r0.gba
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "texkill : Tex14, writemask=rgba";
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
			ps_3_0
			def c0, -0.2, -0.2, -0.2, -0.2
			dcl_texcoord0 v0.xy
            dcl_2d s14
			
			texld r0, v0, s14
			add r0, r0, c0
			texkill r0.rgba
			mov oC0, r0
        };
		VertexShader = <TexTLVS>;
   }
}
