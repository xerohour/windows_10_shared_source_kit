#include "include.fx"
		
Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, Full WriteMask, Any Component < 0";
	String	Shape = "TexTL0";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = None; 
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_0
            def         c0, -0.2, 0, 0, 0
            dcl         t0.xy
            dcl_2d      s0
				
            texld       r0, t0, s0
            add         r0, r0, c0.x
            texkill     r0
            mov         oC0, r0
        };
         
        VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill,  Full WriteMask, .r < 0";
	String	Shape = "TexTL0";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = None; 
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_0
            def         c0, -0.2, 0, 0, 0
            dcl         t0.xy
            dcl_2d      s0
				
            texld       r0, t0, s0
            add         r0.r, r0.r, c0.r
            texkill     r0
            mov         oC0, r0
        };
         
        VertexShader = <TexTLVS>;
    }
}

Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, Full WriteMask, .g < 0";
	String	Shape = "TexTL0";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = None; 
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_0
            def c0, -0.2, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
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
	String	Shape = "TexTL0";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = None; 
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_0
            def c0, -0.2, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
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
	String	Shape = "TexTL0";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = None; 
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_0
            def c0, -0.2, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
            add r0.a, r0.a, c0.r
            texkill r0
            mov oC0, r0
        };
         
        VertexShader = <TexTLVS>;
    }
}


