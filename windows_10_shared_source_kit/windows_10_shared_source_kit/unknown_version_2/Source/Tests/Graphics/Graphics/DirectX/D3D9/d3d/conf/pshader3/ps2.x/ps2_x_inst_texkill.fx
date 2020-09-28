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
            ps_2_x
            def c0, -0.2, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
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
	String	Shape = "TexTL0";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = None; 
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0, -0.2, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
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
	String	Shape = "TexTL0";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = None; 
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
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
            ps_2_x
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
            ps_2_x
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

// !!! texkill author should note that 2.x texkill must use default writemask.  Components are not legal for this version.  See shader spec.
#if 0
Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill,  .r WriteMask, .r < 0";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0,-0.1, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
            add r0.r, r0.r, c0.r
            texkill r0.r
            mov oC0, r0
        };
         
        VertexShader = <StandardVS>;
    }
}

Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, .g WriteMask, .g < 0";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0,-0.1, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
            add r0.g, r0.g, c0.r
            texkill r0.g
            mov oC0, r0
        };
         
        VertexShader = <StandardVS>;
    }
}


Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, .b WriteMask, .b < 0";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0,-0.1, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
            add r0.b, r0.b, c0.r
            texkill r0.b
            mov oC0, r0
        };
         
        VertexShader = <StandardVS>;
    }
}


Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, .a WriteMask, .a < 0";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0,-0.1, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
            add r0.a, r0.a, c0.r
            texkill r0.a
            mov oC0, r0
        };
         
        VertexShader = <StandardVS>;
    }
}

Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, .gba WriteMask, .r < 0";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0,-0.1, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
            add r0.r, r0.r, c0.r
            texkill r0.gba
            mov oC0, r0
        };
         
        VertexShader = <StandardVS>;
    }
}

Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, .gb WriteMask, .r < 0";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0,-0.1, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
            add r0.r, r0.r, c0.r
            texkill r0.gb
            mov oC0, r0
        };
         
        VertexShader = <StandardVS>;
    }
}

Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, .ba WriteMask, .r < 0";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0,-0.1, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
            add r0.r, r0.r, c0.r
            texkill r0.ba
            mov oC0, r0
        };
         
        VertexShader = <StandardVS>;
    }
}

Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, .ga WriteMask, .r < 0";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0,-0.1, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
            add r0.r, r0.r, c0.r
            texkill r0.ga
            mov oC0, r0
        };
         
        VertexShader = <StandardVS>;
    }
}

Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, .g WriteMask, .r < 0";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0,-0.1, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
            add r0.r, r0.r, c0.r
            texkill r0.g
            mov oC0, r0
        };
         
        VertexShader = <StandardVS>;
    }
}

Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, .b WriteMask, .r < 0";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0,-0.1, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
            add r0.r, r0.r, c0.r
            texkill r0.b
            mov oC0, r0
        };
         
        VertexShader = <StandardVS>;
    }
}

Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, .a WriteMask, .r < 0";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        Texture[0] = <Tex2D_0>;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0,-0.1, 0, 0, 0
            dcl t0.xy
            dcl_2d s0
				
            texld r0, t0, s0
            add r0.r, r0.r, c0.r
            texkill r0.a
            mov oC0, r0
        };
         
        VertexShader = <StandardVS>;
    }
}
#endif
