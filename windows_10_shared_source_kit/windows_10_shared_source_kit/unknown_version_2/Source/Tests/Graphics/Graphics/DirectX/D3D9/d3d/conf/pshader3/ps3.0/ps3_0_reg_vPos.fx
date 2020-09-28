#include "include.fx"

//cases for 4 ajusted pixels taking different paths
Technique 
< 
	String	Name = "ps_3_0 vPos.xy: different paths for adjusted pixels in if_gt statement";
	String	Shape = "TexTL";
>
{
    Pass P0 {
		Texture[0] = <Tex2D_0>;     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fTexSizes>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 1, 0.5, 0, -1
            def c2, 0.25, 0, 0, 0
            dcl vPos.xy
            dcl_2d s0
            
			add r0, vPos.x, vPos.y
			mul r1, r0, c0.y
			frc r2, r1 
			mov r1, c0.z
			if_gt r2.x, c2.x
				mov r1, c0.w
				texkill r1
			else
				mov r1.xyw, c0.x
			endif
			mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy: different paths for adjusted pixels in if_lt statement";
	String	Shape = "TexTL";
>
{
    Pass P0 {
		Texture[0] = <Tex2D_0>;     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fTexSizes>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 1, 0.5, 0, -1
            def c2, 0.25, 0, 0, 0
            dcl vPos.xy
            dcl_2d s0
            
			add r0, vPos.x, vPos.y
			mul r1, r0, c0.y
			frc r2, r1 
			mov r1, c0.z
			if_lt r2.x, c2.x
				mov r1, c0.w
				texkill r1
			else
				mov r1.xyw, c0.x
			endif
			mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy: different paths for adjusted pixels in if_ge statement";
	String	Shape = "TexTL";
>
{
    Pass P0 {
		Texture[0] = <Tex2D_0>;     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fTexSizes>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 1, 0.5, 0, -1
            def c2, 0.25, 0, 0, 0
            dcl vPos.xy
            dcl_2d s0
            
			add r0, vPos.x, vPos.y
			mul r1, r0, c0.y
			frc r2, r1 
			mov r1, c0.z
			if_ge r2.x, c2.x
				mov r1.xyw, c0.x
			else
				mov r1, c0.w
				texkill r1
			endif
			mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_3_0 vPos.xy: different paths for adjusted pixels in if_le statement";
	String	Shape = "TexTL";
>
{
    Pass P0 {
		Texture[0] = <Tex2D_0>;     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fTexSizes>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 1, 0.5, 0, -1
            def c2, 0.25, 0, 0, 0
            dcl vPos.xy
            dcl_2d s0
            
			add r0, vPos.x, vPos.y
			mul r1, r0, c0.y
			frc r2, r1 
			mov r1, c0.z
			if_le r2.x, c2.x
				mov r1.xyw, c0.x
			else
				mov r1, c0.w
				texkill r1
			endif
			mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : abs vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
		Texture[0] = <Tex2D_0>;     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fTexSizes>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 1, 0, 0, 1
            dcl vPos.x
            dcl_2d s0
            
			abs r0, vPos.x
			sub r1, vPos.x, r0
			texkill r1
			mov oC0, c0
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : abs vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
		Texture[0] = <Tex2D_0>;     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fTexSizes>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 1, 0, 0, 1
            dcl vPos.xy
            dcl_2d s0
            
			abs r0, vPos.x
			sub r1, vPos.x, r0
			texkill r1
			mov oC0, c0
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : abs vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
		Texture[0] = <Tex2D_0>;     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fTexSizes>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 0, 1, 0, 1
            dcl vPos.y
            dcl_2d s0
            
			abs r0, vPos.y
			sub r1, vPos.y, r0
			texkill r1
			mov oC0, c0
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : abs vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
		Texture[0] = <Tex2D_0>;     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fTexSizes>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 0, 1, 0, 1
            dcl vPos.xy
            dcl_2d s0
            
			abs r0, vPos.y
			sub r1, vPos.y, r0
			texkill r1
			mov oC0, c0
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : abs vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {
		Texture[0] = <Tex2D_0>;     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fTexSizes>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 1, 1, 0, 1
            dcl vPos.xy
            dcl_2d s0
            
			abs r0, vPos.xy
			sub r1, vPos.xy, r0
			texkill r1
			mov oC0, c0
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : add X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0
            def c0, 256, 640, 0, 0
            dcl vPos.x
            
            mul r0, v0.x, c0.y
            add r0, r0, vPos.x
            rcp r1, c0.x
            mul r2, r1, r0
            frc r2, r2
            mul r2, r2, c0.x
            mul r2, r2, r1
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : add X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.xy
            
            mul r0, v0.x, c0.y
            add r0, r0, vPos.x
            rcp r1, c0.x
            mul r2, r1, r0
            frc r2, r2
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : add vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.x
            
            mul r0, v0.x, c0.y
            add r0, vPos.x, r0
            rcp r1, c0.x
            mul r2, r1, r0
            frc r2, r2
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : add vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.xy
            
            mul r0, v0.x, c0.y
            add r0, vPos.x, r0
            rcp r1, c0.x
            mul r2, r1, r0
            frc r2, r2
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : add X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.y
            
            mul r0, v0.y, c0.y
            add r0, r0, vPos.y
            rcp r1, c0.x
            mul r2, r1, r0
            frc r2, r2
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : add X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.xy
            
            mul r0, v0.y, c0.y
            add r0, r0, vPos.y
            rcp r1, c0.x
            mul r2, r1, r0
            frc r2, r2
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : add vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.y
            
            mul r0, v0.y, c0.y
            add r0, vPos.y, r0
            rcp r1, c0.x
            mul r2, r1, r0
            frc r2, r2
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : add vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.xy
            
            mul r0, v0.y, c0.y
            add r0, vPos.y, r0
            rcp r1, c0.x
            mul r2, r1, r0
            frc r2, r2
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : add X, vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.xy
            
            mul r0, v0.xy, c0.y
            add r0, r0, vPos.xy
            rcp r1, c0.x
            mul r2, r1, r0
            frc r2, r2
            mov r2.z, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : add vPos.xy, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.xy
            
            mul r0, v0.xy, c0.y
            add r0, vPos.xy, r0
            rcp r1, c0.x
            mul r2, r1, r0
            frc r2, r2
            mov r2.z, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : break_eq vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.x
            
            mov r0, vPos.x
            mov r1.xw, c0.x
            mov r1.yz, c0.z
            loop aL, i0
				break_eq r0.x, vPos.x // compare to ro=vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_eq vPos.x, r0.x // compare to ro=vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_eq vPos.x, vPos.x //compare to itself
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : break_eq vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.xy
            
            mov r0, vPos.xy
            mov r1.xw, c0.x
            mov r1.yz, c0.z
            loop aL, i0
				break_eq r0.x, vPos.x // compare to ro=vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_eq vPos.x, r0.x // compare to ro=vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_eq vPos.x, vPos.x //compare to itself
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : break_eq vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.y
            
            mov r0, vPos.y
            mov r1.yw, c0.x
            mov r1.xz, c0.z
            loop aL, i0
				break_eq r0.y, vPos.y // compare to r0=vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_eq vPos.y, r0.y // compare to r0=vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_eq vPos.y, vPos.y //compare to itself
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : break_eq vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.xy
            
            mov r0, vPos.xy
            mov r1.yw, c0.x
            mov r1.xz, c0.z
            loop aL, i0
				break_eq r0.y, vPos.y // compare to r0=vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_eq vPos.y, r0.y // compare to r0=vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_eq vPos.y, vPos.y //compare to itself
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : break_ne vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.x
            
            mov r0, vPos.xxxx
            add r0.xy, r0, c0.x
            add r0.zw, r0, c0.y
            mov r1.xw, c0.x
            mov r1.yz, c0.z
            loop aL, i0
				break_ne r0.x, vPos.x //compare to r0=vPos+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ne r0.z, vPos.x //compare to r0=vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ne vPos.x, r0.x //compare to r0=vPos+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ne vPos.x, r0.z //compare to r0=vPos-1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : break_ne vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.xy
            
            mov r0, vPos.xyxy
            add r0.xy, r0, c0.x
            add r0.zw, r0, c0.y
            mov r1.xw, c0.x
            mov r1.yz, c0.z
            loop aL, i0
				break_ne r0.x, vPos.x //compare to r0=vPos+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ne r0.z, vPos.x //compare to r0=vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ne vPos.x, r0.x //compare to r0=vPos+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ne vPos.x, r0.z //compare to r0=vPos-1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : break_ne vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.y
            
            mov r0, vPos.yyyy
            add r0.xy, r0, c0.x
            add r0.zw, r0, c0.y
            mov r1.yw, c0.x
            mov r1.xz, c0.z
            loop aL, i0
				break_ne r0.y, vPos.y //compare to r0=vPos+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ne r0.w, vPos.y //compare to r0=vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ne vPos.y, r0.y //compare to r0=vPos+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ne vPos.y, r0.w //compare to r0=vPos-1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : break_ne vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.xy
            
            mov r0, vPos.xyxy
            add r0.xy, r0, c0.x
            add r0.zw, r0, c0.y
            mov r1.yw, c0.x
            mov r1.xz, c0.z
            loop aL, i0
				break_ne r0.y, vPos.y //compare to r0=vPos+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ne r0.w, vPos.y //compare to r0=vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ne vPos.y, r0.y //compare to r0=vPos+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ne vPos.y, r0.w //compare to r0=vPos-1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : break_gt vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.x
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.x, c0.y
            add r2.zw, vPos.x, c0.x
            mov r1.xw, c0.x
            mov r1.yz, c0.z
            loop aL, i0
				break_gt vPos.x, r0.x //compare to Xmin-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_gt r0.z, vPos.x //compare to Xmax+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_gt vPos.x, r2.x //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_gt r2.z, vPos.x //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : break_gt vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.xy
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.xy, c0.y
            add r2.zw, vPos.xyxy, c0.x
            mov r1.xw, c0.x
            mov r1.yz, c0.z
            loop aL, i0
				break_gt vPos.x, r0.x //compare to Xmin-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_gt r0.z, vPos.x //compare to Xmax+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_gt vPos.x, r2.x //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_gt r2.z, vPos.x //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : break_gt vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.y
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.y, c0.y
            add r2.zw, vPos.y, c0.x
            mov r1.yw, c0.x
            mov r1.xz, c0.z
            loop aL, i0
				break_gt vPos.y, r0.y //compare to Ymin-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_gt r0.w, vPos.y //compare to Ymax+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_gt vPos.y, r2.y //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_gt r2.w, vPos.y //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : break_gt vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.xy
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.xy, c0.y
            add r2.zw, vPos.xyxy, c0.x
            mov r1.yw, c0.x
            mov r1.xz, c0.z
            loop aL, i0
				break_gt vPos.y, r0.y //compare to Ymin-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_gt r0.w, vPos.y //compare to Ymax+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_gt vPos.y, r2.y //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_gt r2.w, vPos.y //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : break_lt vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.x
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.x, c0.y
            add r2.zw, vPos.x, c0.x
            mov r1.xw, c0.x
            mov r1.yz, c0.z
            loop aL, i0
				break_lt r0.x, vPos.x //compare to Xmin-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_lt vPos.x, r0.z //compare to Xmax+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_lt r2.x, vPos.x //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_lt vPos.x, r2.z //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : break_lt vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.xy
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.xy, c0.y
            add r2.zw, vPos.xyxy, c0.x
            mov r1.xw, c0.x
            mov r1.yz, c0.z
            loop aL, i0
				break_lt r0.x, vPos.x //compare to r0=Xmin-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_lt vPos.x, r0.z //compare to r0=Xmax+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_lt r2.x, vPos.x //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_lt vPos.x, r2.z //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : break_lt vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.y
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
			mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.y, c0.y
            add r2.zw, vPos.y, c0.x
            mov r1.yw, c0.x
            mov r1.xz, c0.z
            loop aL, i0
				break_lt r0.y, vPos.y //compare to r0=Ymin-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_lt vPos.y, r0.w //compare to r0=Ymax+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_lt r2.y, vPos.y //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_lt vPos.y, r2.w //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : break_lt vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.xy
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.xy, c0.y
            add r2.zw, vPos.xyxy, c0.x
            mov r1.yw, c0.x
            mov r1.xz, c0.z
            loop aL, i0
				break_lt r0.y, vPos.y //compare to r0=Ymin-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_lt vPos.y, r0.w //compare to r0=Ymax+1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_lt r2.y, vPos.y //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_lt vPos.y, r2.w //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : break_ge vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.x
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.x, c0.y
            add r2.zw, vPos.x, c0.x
            mov r1.xw, c0.x
            mov r1.yz, c0.z
            loop aL, i0
				break_ge vPos.x, c1.x //compare to Xmin
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge c1.z, vPos.x //compare to Xmax
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge vPos.x, vPos.x //compare to vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge vPos.x, r2.x //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge r2.z, vPos.x //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : break_ge vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.xy
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.xy, c0.y
            add r2.zw, vPos.xyxy, c0.x
            mov r1.xw, c0.x
            mov r1.yz, c0.z
            loop aL, i0
				break_ge vPos.x, c1.x //compare to Xmin
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge c1.z, vPos.x //compare to Xmax
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge vPos.x, vPos.x //compare to vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge vPos.x, r2.x //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge r2.z, vPos.x //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : break_ge vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.y
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.y, c0.y
            add r2.zw, vPos.y, c0.x
            mov r1.yw, c0.x
            mov r1.xz, c0.z
            loop aL, i0
				break_ge vPos.y, c1.y //compare to Ymin
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge c1.w, vPos.y //compare to Ymax
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge vPos.y, vPos.y //compare to vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge vPos.y, r2.y //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge r2.w, vPos.y //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : break_ge vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.xy
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.xy, c0.y
            add r2.zw, vPos.xyxy, c0.x
            mov r1.yw, c0.x
            mov r1.xz, c0.z
            loop aL, i0
				break_ge vPos.y, c1.y //compare to Ymin
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge c1.w, vPos.y //compare to Ymax
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge vPos.y, vPos.y //compare to vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge vPos.y, r2.y //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_ge r2.w, vPos.y //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : break_le vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.x
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.x, c0.y
            add r2.zw, vPos.x, c0.x
            mov r1.xw, c0.x
            mov r1.yz, c0.z
            loop aL, i0
				break_le c1.x, vPos.x //compare to Xmin
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le vPos.x, c1.z //compare to Xmax
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le vPos.x, vPos.x //compare to vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le r2.x, vPos.x //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le vPos.x, r2.z //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : break_le vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.xy
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.xy, c0.y
            add r2.zw, vPos.xyxy, c0.x
            mov r1.xw, c0.x
            mov r1.yz, c0.z
            loop aL, i0
				break_le c1.x, vPos.x //compare to r0=Xmin
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le vPos.x, c1.z //compare to r0=Xmax
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le vPos.x, vPos.x //compare to vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le r2.x, vPos.x //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le vPos.x, r2.z //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : break_le vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.y
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.y, c0.y
            add r2.zw, vPos.y, c0.x
            mov r1.yw, c0.x
            mov r1.xz, c0.z
            loop aL, i0
				break_le c1.y, vPos.y //compare to r0=Ymin
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le vPos.y, c1.w //compare to r0=Ymax
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le vPos.y, vPos.y //compare to vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le r2.y, vPos.y //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le vPos.y, r2.w //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : break_le vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.xy
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.xy, c0.y
            add r2.zw, vPos.xyxy, c0.x
            mov r1.yw, c0.x
            mov r1.xz, c0.z
            loop aL, i0
				break_le c1.y, vPos.y //compare to r0=Ymin
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le vPos.y, c1.w //compare to r0=Ymax
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le vPos.y, vPos.y //compare to vPos
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le r2.y, vPos.y //compare to vPos-1
				mov r1, c0.y
            endloop
            loop aL, i0
				break_le vPos.y, r2.w //compare to vPos+1
				mov r1, c0.y
            endloop
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : cmp vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.x
            
            cmp r0, vPos.x, c0.xzzx, c0.y //compare to 0
            texkill r0
            mov oC0, r0
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : cmp vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.xy
            
            cmp r0, vPos.x, c0.xzzx, c0.y //compare to 0
            texkill r0
            mov oC0, r0
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : cmp vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.y
            
            cmp r0, vPos.y, c0.zxzx, c0.y //compare to 0
            texkill r0
            mov oC0, r0
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : cmp vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.xy
            
            cmp r0, vPos.y, c0.zxzx, c0.y //compare to 0
            texkill r0
            mov oC0, r0
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : cmp vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.xy
            
            cmp r0, vPos.xy, c0.xxzx, c0.y //compare to 0
            texkill r0
            mov oC0, r0
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : dp2add vPos.x, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.x
            
            mov r1, c0
            rcp r1.x, c0.x
            mul r0, r1.x, c1
            //(vPos.x-Xmin)%12.8 where 12.8 = 256/20
            dp2add r0.xw, vPos.x, r1.xz, -r0.x
            frc r2, r0
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp2add vPos.x, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            mul r0, r1.x, c1
            //(vPos.x-Xmin)%12.8 where 12.8 = 256/20
            dp2add r0.xw, vPos.x, r1.xz, -r0.x
            frc r2, r0
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : dp2add X, vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.x
            
            mov r1, c0
            rcp r1.x, c0.x
            mul r0, r1.x, c1
            //(vPos.x-Xmin)%12.8 where 12.8 = 256/20
            dp2add r0.xw, r1.xz, vPos.x, -r0.x
            frc r2, r0
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp2add X, vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            mul r0, r1.x, c1
            //(vPos.x-Xmin)%12.8 where 12.8 = 256/20
            dp2add r0.xw, r1.xz, vPos.x, -r0.x
            frc r2, r0
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : dp2add X, X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.x
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin)%(12.8) where 12.8=256/20
            dp2add r0.xw, r1.yz, c1.xy, vPos.x
            mul r0, r0, r1.x
            frc r2, r0
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp2add X, X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin)%12.8 where 12.8=256/20
            dp2add r0.xw, r1.yz, c1.xy, vPos.x
            mul r0, r0, r1.x
            frc r2, r0
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : dp2add vPos.y, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.y
            
            mov r1, c0
            rcp r1.x, c0.x
            mul r0, r1.x, c1
            //(vPos.y-Ymin)%12.8 where 12.8=256/20
            dp2add r0.yw, vPos.y, r1.xz, -r0.y
            frc r2, r0
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp2add vPos.y, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            mul r0, r1.x, c1
            //(vPos.y-Ymin)%12.8 where 12.8=256/20
            dp2add r0.yw, vPos.y, r1.xz, -r0.y
            frc r2, r0
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : dp2add X, vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.y
            
            mov r1, c0
            rcp r1.x, c0.x
            mul r0, r1.x, c1
            //(vPos.y-Ymin)%12.8 where 12.8=256/20
            dp2add r0.yw, r1.xz, vPos.y, -r0.y
            frc r2, r0
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp2add X, vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            mul r0, r1.x, c1
            //(vPos.y-Ymin)%12.8 where 12.8=256/20
            dp2add r0.yw, r1.xz, vPos.y, -r0.y
            frc r2, r0
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : dp2add X, X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.y
            
            mov r1, c0.xzy
            rcp r1.x, c0.x
            //(vPos.y-Ymin)%12.8 where 12.8=256/20
            dp2add r0.yw, r1.yz, c1.xy, vPos.y
            mul r0, r0, r1.x
            frc r2, r0
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp2add X, X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0.xzy
            rcp r1.x, c0.x
            //(vPos.y-Ymin)%12.8 where 12.8=256/20
            dp2add r0.yw, r1.yz, c1.xy, vPos.y
            mul r0, r0, r1.x
            frc r2, r0
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp2add X, vPos.xy, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 25.6, -1, 0, 0
            dcl vPos.xy
            
            rcp r1, c0.x
            mul r0, r1.x, c1
            add r0.x, r0.x, r0.y
            //(vPos.x+vPos.y-Xmin-Ymin)%25.6
            dp2add r0.xyw, r1.x, vPos.xy, -r0.x
            frc r2, r0
            mov r2.z, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp2add vPos.xy, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 25.6, -1, 0, 0
            dcl vPos.xy
            
            rcp r1, c0.x
            mul r0, r1.x, c1
            add r0.x, r0.x, r0.y
            //(vPos.x+vPos.y-Xmin-Ymin)%25.6
            dp2add r0.xyw, vPos.xy, r1.x, -r0.x
            frc r2, r0
            mov r2.z, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : dp3 vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.x
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp3 r0.xw, vPos.x, r1.xzzz
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp3 vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp3 r0.xw, vPos.x, r1.xzzz
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : dp3 X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.x
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp3 r0.xw, r1.xzzz, vPos.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp3 X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp3 r0.xw, r1.xzzz, vPos.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : dp3 vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.y
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp3 r0.yw, vPos.y, r1.zxzz
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp3 vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp3 r0.yw, vPos.xy, r1.zxzz
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : dp3 X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.y
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp3 r0.yw, r1.zxzz, vPos.y
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp3 X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp3 r0.yw, r1.zxzz, vPos.y
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp3 vPos.xy,X ";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp3 r0.xyw, vPos.xy, r1.xxzz
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp3 X, vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp3 r0.xyw, r1.xxzz, vPos.xy
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : dp4 vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.x
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp4 r0.xw, vPos.x, r1.xzzz
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp4 vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp4 r0.xw, vPos.x, r1.xzzz
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : dp4 X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.x
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp4 r0.xw, r1.xzzz, vPos.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp4 X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp4 r0.xw, r1.xzzz, vPos.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : dp4 vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.y
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp4 r0.yw, vPos.y, r1.zxzz
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp4 vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp4 r0.yw, vPos.xy, r1.zxzz
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : dp4 X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.y
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp4 r0.yw, r1.zxzz, vPos.y
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp4 X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp4 r0.yw, r1.zxzz, vPos.y
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp4 vPos.xy,X ";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp4 r0.xyw, vPos.xy, r1.xxzz
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dp4 X, vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x+vPos.y)%12.8 where 12.8=256/20
            dp4 r0.xyw, r1.xxzz, vPos.xy
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : dsx vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl vPos.x
            
            dsx r2, vPos.x
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dsx vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl vPos.xy
            
            dsx r2, vPos.x
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : dsx vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl vPos.y
            
            dsx r2, vPos.y
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dsx vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl vPos.xy
            
            dsx r2, vPos.y
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dsx vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl vPos.xy
            
            dsx r2, vPos.xy
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}




Technique 
< 
	String	Name = "ps_3_0 vPos.x : dsy vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl vPos.x
            
            dsy r2, vPos.x
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dsy vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl vPos.xy
            
            dsy r2, vPos.x
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : dsy vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl vPos.y
            
            dsy r2, vPos.y
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dsy vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl vPos.xy
            
            dsy r2, vPos.y
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : dsy vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl vPos.xy
            
            dsy r2, vPos.xy
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : exp vPos.x";
	String	Shape = "Quad";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 100, -1, 0
            dcl vPos.x
            
            rcp r1, c0.x
			mov r0, c0.w
			if_gt vPos.x, c0.y
				mov r0, c0.z
			else
				exp r0.xw, vPos.x
				log r0.xw, r0.x
			endif
			texkill r0
			mul r0, r0, r1.x
			frc r1, r0
			mov oC0, r1
			
        };
         
        VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : exp vPos.x";
	String	Shape = "Quad";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 100, -1, 0
            dcl vPos.xy
            
			rcp r1, c0.x
			mov r0, c0.w
			if_gt vPos.x, c0.y
				mov r0, c0.z
			else
				exp r0.xw, vPos.x
				log r0.xw, r0.x
			endif
			texkill r0
			mul r0, r0, r1.x
			frc r1, r0
			mov oC0, r1
        };
         
        VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : exp vPos.y";
	String	Shape = "Quad";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 100, -1, 0
            dcl vPos.y
            
			rcp r1, c0.x
			mov r0, c0.w
			if_gt vPos.y, c0.y
				mov r0, c0.z
			else
				exp r0.yw, vPos.y
				log r0.yw, r0.y
			endif
			texkill r0
			mul r0, r0, r1.x
			frc r1, r0
			mov oC0, r1
        };
         
        VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : exp vPos.y";
	String	Shape = "Quad";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 100, -1, 0
            dcl vPos.xy
            
			rcp r1, c0.x
			mov r0, c0.w
			if_gt vPos.y, c0.y
				mov r0, c0.z
			else
				exp r0.yw, vPos.y
				log r0.yw, r0.y
			endif
			texkill r0
			mul r0, r0, r1.x
			frc r1, r0
			mov oC0, r1
        };
         
        VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : frc vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.x
            
            frc r0, vPos.x
            mov r1, c0.xzzx
            sub r2, r0, c0.z
            sub r3, c0.z, r0
            cmp r1, r2, r1, c0.y
            cmp r1, r3, r1, c0.y
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : frc vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.xy
            
            frc r0, vPos.x
            mov r1, c0.xzzx
            sub r2, r0, c0.z
            sub r3, c0.z, r0
            cmp r1, r2, r1, c0.y
            cmp r1, r3, r1, c0.y
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : frc vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.y
            
            frc r0, vPos.y
            mov r1, c0.zxzx
            sub r2, r0, c0.z
            sub r3, c0.z, r0
            cmp r1, r2, r1, c0.y
            cmp r1, r3, r1, c0.y
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : frc vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.xy
            
            frc r0, vPos.y
            mov r1, c0.zxzx
            sub r2, r0, c0.z
            sub r3, c0.z, r0
            cmp r1, r2, r1, c0.y
            cmp r1, r3, r1, c0.y
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : frc vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.xy
            
            frc r0, vPos.xy
            mov r1, c0.xxzx
            sub r2, r0, c0.z
            sub r3, c0.z, r0
            cmp r1, r2, r1, c0.y
            cmp r1, r3, r1, c0.y
            texkill r1
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : if_eq vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.x
            
            mov r0, vPos.x
            mov r2.xw, c0.x
            mov r2.yz, c0.z
            add r3, vPos.x, c0.y
			if_eq r0.x, vPos.x // compare to ro=vPos
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
			if_eq vPos.x, r3.x // compare to ro=vPos-1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r2
            endif
			if_eq vPos.x, vPos.x //compare to itself
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : if_eq vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, 1, 0, 0
            dcl vPos.xy
            
            mov r0, vPos.x
            mov r2.xw, c0.x
            mov r2.yz, c0.z
            add r3, vPos.x, c0.y
			if_eq r3.x, vPos.x // compare to ro=vPos+1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r2
            endif
			if_eq vPos.x, r0.x // compare to ro=vPos
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
			if_eq vPos.x, vPos.x //compare to itself
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : if_eq vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.y
            
            mov r0, vPos.y
            mov r2.yw, c0.x
            mov r2.xz, c0.z
            add r3, vPos.y, c0.y
			if_eq r0.y, vPos.y // compare to ro=vPos
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
			if_eq vPos.y, r3.y // compare to ro=vPos-1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r2
            endif
			if_eq vPos.y, vPos.y //compare to itself
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : if_eq vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, 1, 0, 0
            dcl vPos.xy
            
            mov r0, vPos.y
            mov r2.yw, c0.x
            mov r2.xz, c0.z
            add r3, vPos.y, c0.y
			if_eq r3.y, vPos.y // compare to ro=vPos+1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r2
            endif
			if_eq vPos.y, r0.y // compare to ro=vPos
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
			if_eq vPos.y, vPos.y //compare to itself
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : if_ne vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, 1, 0, 0
            dcl vPos.x
            
            mov r0, vPos.x
            mov r2.xw, c0.x
            mov r2.yz, c0.z
            add r3, vPos.x, c0.y
            add r4, vPos.x, -c0.y
			if_ne r3.x, vPos.x // compare to ro=vPos+1
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
            if_ne vPos.x, r4.x // compare to ro=vPos-1
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
			if_ne vPos.x, r0.x // compare to ro=vPos
				mov r1, c0.y
				texkill r1
			else
				mov r1, r2
            endif
			if_ne vPos.x, vPos.x //compare to itself
				mov r1, c0.y
				texkill r1
			else
				mov r1, r2
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : if_ne vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, 1, 0, 0
            dcl vPos.xy
            
            mov r0, vPos.x
            mov r2.xw, c0.x
            mov r2.yz, c0.z
            add r3, vPos.x, -c0.y
            add r4, vPos.x,  c0.y
			if_ne r3.x, vPos.x // compare to ro=vPos-1
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
            if_ne vPos.x, r4.x // compare to ro=vPos+1
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
			if_ne r0.x, vPos.x // compare to ro=vPos
				mov r1, c0.y
				texkill r1
			else
				mov r1, r2
            endif
			if_ne vPos.x, vPos.x //compare to itself
				mov r1, c0.y
				texkill r1
			else
				mov r1, r2
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : if_ne vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, 1, 0, 0
            dcl vPos.y
            
            mov r0, vPos.y
            mov r2.yw, c0.x
            mov r2.xz, c0.z
            add r3, vPos.y, -c0.y
            add r4, vPos.y,  c0.y
			if_ne r3.y, vPos.y // compare to ro=vPos-1
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
            if_ne vPos.y, r4.y // compare to ro=vPos+1
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
			if_ne r0.y, vPos.y // compare to ro=vPos
				mov r1, c0.y
				texkill r1
			else
				mov r1, r2
            endif
			if_ne vPos.y, vPos.y //compare to itself
				mov r1, c0.y
				texkill r1
			else
				mov r1, r2
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : if_ne vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, 1, 0, 0
            dcl vPos.xy
            
            mov r0, vPos.y
            mov r2.yw, c0.x
            mov r2.xz, c0.z
            add r3, vPos.y, c0.y
            add r4, vPos.y, -c0.y
			if_ne r3.y, vPos.y // compare to ro=vPos+1
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
            if_ne vPos.y, r4.y // compare to ro=vPos-1
				mov r1, r2
			else
				mov r1, c0.y
				texkill r1
            endif
			if_ne vPos.y, r0.y // compare to ro=vPos
				mov r1, c0.y
				texkill r1
			else
				mov r1, r2
            endif
			if_ne vPos.y, vPos.y //compare to itself
				mov r1, c0.y
				texkill r1
			else
				mov r1, r2
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : if_gt vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.x
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.x, c0.y
            add r2.zw, vPos.x, c0.x
            mov r3.xw, c0.x
            mov r3.yz, c0.z
            if_gt vPos.x, r0.x //compare to Xmin-1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_gt r0.z, vPos.x //compare to Xmax+1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_gt r2.x, vPos.x //compare to vPos-1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_gt vPos.x, r2.z //compare to vPos+1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_gt vPos.x, vPos.x //compare to itself
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : if_gt vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.xy
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.x, c0.y
            add r2.zw, vPos.x, c0.x
            mov r3.xw, c0.x
            mov r3.yz, c0.z
            if_gt r0.x, vPos.x //compare to Xmin-1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_gt vPos.x, r0.z //compare to Xmax+1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_gt vPos.x, r2.x //compare to vPos-1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_gt r2.z, vPos.x //compare to vPos+1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_gt vPos.x, vPos.x //compare to itself
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : if_gt vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.y
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.y, c0.y
            add r2.zw, vPos.y, c0.x
            mov r3.yw, c0.x
            mov r3.xz, c0.z
            if_gt r0.y, vPos.y //compare to Xmin-1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_gt vPos.y, r0.w //compare to Xmax+1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_gt vPos.y, r2.y //compare to vPos-1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_gt r2.w, vPos.y //compare to vPos+1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_gt vPos.y, vPos.y //compare to itself
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : if_gt vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.xy
            
           //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.y, c0.y
            add r2.zw, vPos.y, c0.x
            mov r3.yw, c0.x
            mov r3.xz, c0.z
            if_gt vPos.y, r0.y //compare to Xmin-1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_gt r0.w, vPos.y //compare to Xmax+1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_gt r2.y, vPos.y //compare to vPos-1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_gt vPos.y, r2.w //compare to vPos+1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_gt vPos.y, vPos.y //compare to itself
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : if_lt vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.x
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.x, c0.y
            add r2.zw, vPos.x, c0.x
            mov r3.xw, c0.x
            mov r3.yz, c0.z
            
			if_lt r0.x, vPos.x //compare to Xmin-1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_lt vPos.x, r0.z //compare to Xmax+1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_lt vPos.x, r2.x //compare to vPos-1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_lt r2.z, vPos.x //compare to vPos+1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_lt vPos.x, vPos.x //compare to itself
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : if_lt vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.xy
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.x, c0.y
            add r2.zw, vPos.x, c0.x
            mov r3.xw, c0.x
            mov r3.yz, c0.z
            if_lt vPos.x, r0.x //compare to Xmin-1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_lt r0.z, vPos.x //compare to Xmax+1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_lt r2.x, vPos.x //compare to vPos-1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_lt vPos.x, r2.z //compare to vPos+1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_lt vPos.x, vPos.x //compare to itself
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : if_lt vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.y
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.y, c0.y
            add r2.zw, vPos.y, c0.x
            mov r3.yw, c0.x
            mov r3.xz, c0.z
            if_lt vPos.y, r0.y //compare to Xmin-1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_lt r0.w, vPos.y //compare to Xmax+1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_lt r2.y, vPos.y //compare to vPos-1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_lt vPos.y, r2.w //compare to vPos+1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_lt vPos.y, vPos.y //compare to itself
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : if_lt vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.xy
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            mov r1, c0
            add r0.xy, c1, r1.y
            add r0.zw, c1, r1.x
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.y, c0.y
            add r2.zw, vPos.y, c0.x
            mov r3.yw, c0.x
            mov r3.xz, c0.z
            if_lt r0.y, vPos.y //compare to Xmin-1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_lt vPos.y, r0.w //compare to Xmax+1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_lt vPos.y, r2.y //compare to vPos-1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_lt r2.w, vPos.y //compare to vPos+1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_lt vPos.y, vPos.y //compare to itself
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : if_ge vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.x
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.x, c0.y
            add r2.zw, vPos.x, c0.x
            mov r3.xw, c0.x
            mov r3.yz, c0.z
            if_ge c1.x, vPos.x //compare to Xmin
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_ge vPos.x, c1.z //compare to Xmax
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_ge vPos.x, r2.x //compare to vPos-1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_ge r2.z, vPos.x //compare to vPos+1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_ge vPos.x, vPos.x //compare to vPos
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : if_ge vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.xy
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.x, c0.y
            add r2.zw, vPos.x, c0.x
            mov r3.xw, c0.x
            mov r3.yz, c0.z
            if_ge vPos.x, c1.x //compare to Xmin
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_ge c1.z, vPos.x //compare to Xmax
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_ge r2.x, vPos.x //compare to vPos-1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_ge vPos.x, r2.z //compare to vPos+1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_ge vPos.x, vPos.x //compare to vPos
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : if_ge vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.y
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.y, c0.y
            add r2.zw, vPos.y, c0.x
            mov r3.yw, c0.x
            mov r3.xz, c0.z
            if_ge vPos.y, c1.y //compare to Xmin
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_ge c1.w, vPos.y //compare to Xmax
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_ge r2.y, vPos.y //compare to vPos-1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_ge vPos.y, r2.w //compare to vPos+1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_ge vPos.y, vPos.y //compare to vPos
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : if_ge vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.xy
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.y, c0.y
            add r2.zw, vPos.y, c0.x
            mov r3.yw, c0.x
            mov r3.xz, c0.z
            if_ge c1.y, vPos.y //compare to Xmin
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_ge vPos.y, c1.w //compare to Xmax
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_ge vPos.y, r2.y //compare to vPos-1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_ge r2.w, vPos.y //compare to vPos+1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_ge vPos.y, vPos.y //compare to vPos
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : if_le vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.x
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.x, c0.y
            add r2.zw, vPos.x, c0.x
            mov r3.xw, c0.x
            mov r3.yz, c0.z
            if_le c1.x, vPos.x //compare to Xmin
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_le vPos.x, c1.z //compare to Xmax
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_le vPos.x, r2.x //compare to vPos-1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_le r2.z, vPos.x //compare to vPos+1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_le vPos.x, vPos.x //compare to vPos
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : if_le vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.xy
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.x, c0.y
            add r2.zw, vPos.x, c0.x
            mov r3.xw, c0.x
            mov r3.yz, c0.z
            if_le vPos.x, c1.x //compare to Xmin
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_le c1.z, vPos.x //compare to Xmax
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_le r2.x, vPos.x //compare to vPos-1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_le vPos.x, r2.z //compare to vPos+1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_le vPos.x, vPos.x //compare to vPos
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : if_le vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            dcl vPos.y
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.y, c0.y
            add r2.zw, vPos.y, c0.x
            mov r3.yw, c0.x
            mov r3.xz, c0.z
            if_le vPos.y, c1.y //compare to Xmin
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_le c1.w, vPos.y //compare to Xmax
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_le r2.y, vPos.y //compare to vPos-1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_le vPos.y, r2.w //compare to vPos+1
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_le vPos.y, vPos.y //compare to vPos
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : if_le vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 1, -1, 0, 0
            defi i0, 1, 0, 1, 0
            dcl vPos.xy
            
            //c1.xyzw=(Xmin,Ymin,Xmax,Ymax)
            //r2.xy=vPos.x-1,vPos.y-1,vPos.x+1,vPos.y+1)
            add r2.xy, vPos.y, c0.y
            add r2.zw, vPos.y, c0.x
            mov r3.yw, c0.x
            mov r3.xz, c0.z
            if_le c1.y, vPos.y //compare to Xmin
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_le vPos.y, c1.w //compare to Xmax
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            if_le vPos.y, r2.y //compare to vPos-1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_le r2.w, vPos.y //compare to vPos+1
				mov r1, c0.y
				texkill r1
			else
				mov r1, r3
            endif
            if_le vPos.y, vPos.y //compare to vPos
				mov r1, r3
            else
				mov r1, c0.y
				texkill r1
            endif
            mov oC0, r1
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : log vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 2000, 0, 0
            dcl vPos.x
            
            mov r2.yz, c0.z
            rcp r1.x, c0.x
            log r2.xw, vPos.x
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : log vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 2000, 0, 0
            dcl vPos.xy
            
            mov r2.yz, c0.z
            rcp r1.x, c0.x
            log r2.xw, vPos.x
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : log vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 2000, 0, 0
            dcl vPos.y
            
            mov r2.xz, c0.z
            rcp r1.x, c0.x
            log r2.yw, vPos.y
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : log vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 2000, 0, 0
            dcl vPos.xy
            
            mov r2.xz, c0.z
            rcp r1.x, c0.x
            log r2.yw, vPos.y
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : lrp vPos.x, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 0, 0, 0
            dcl vPos.x
            
            mov r2.yz, c0.z
            rcp r1.x, c0.x
            lrp r2.xw, vPos.x, c1.z, c1.x
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : lrp vPos.x, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 0, 0, 0
            dcl vPos.xy
            
            mov r2.yz, c0.z
            rcp r1.x, c0.x
            lrp r2.xw, vPos.x, c1.x, c1.z
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : lrp vPos.y, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 0, 0, 0
            dcl vPos.y
            
            mov r2.xz, c0.z
            rcp r1.x, c0.x
            lrp r2.yw, vPos.y, c1.y, c1.w
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : lrp vPos.y, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 0, 0, 0
            dcl vPos.xy
            
            mov r2.xz, c0.z
            rcp r1.x, c0.x
            lrp r2.yw, vPos.y, c1.w, c1.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : lrp vPos.xy, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 0, 0, 0
            dcl vPos.xy
            
            mov r2.z, c0.z
            rcp r1.x, c0.x
            lrp r2.xyw, vPos.xy, c1.zw, c1.xy
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : lrp vPos.xy, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 0, 0, 0
            dcl vPos.xy
            
            mov r2.z, c0.z
            rcp r1.x, c0.x
            lrp r2.xyw, vPos.xy, c1.xy, c1.zw
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : lrp X, vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.x
            
            mov r2.yz, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            lrp r2.xw, r1.y, vPos.x, c1.x
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : lrp X, vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.yz, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            lrp r2.xw, r1.y, vPos.x, c1.z
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : lrp X, vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.y
            
            mov r2.xz, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            lrp r2.yw, r1.y, vPos.y, c1.w
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : lrp X, vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.xz, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            lrp r2.yw, r1.y, vPos.y, c1.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : lrp X, vPos.xy, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.z, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            lrp r2.xyw, r1.y, vPos.xy, c1.xy
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : lrp X, vPos.xy, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.z, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            lrp r2.xyw, r1.y, vPos.xy, c1.zw
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : lrp X, X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.x
            
            mov r2.yz, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            lrp r2.xw, r1.y, c1.z, vPos.x
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : lrp X, X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.yz, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            lrp r2.xw, r1.y, c1.x, vPos.x
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : lrp X, X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.y
            
            mov r2.xz, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            lrp r2.yw, r1.y, c1.y, vPos.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : lrp X, X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.xz, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            lrp r2.yw, r1.y, c1.w, vPos.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : lrp X, X, vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.z, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            lrp r2.xyw, r1.y, c1.xy, vPos.xy
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : lrp X, X, vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.z, c0.z
            mov r1, c0
            rcp r1.x, c0.x
            lrp r2.xyw, r1.y, c1.zw, vPos.xy
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : m3x2 vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20, 0, 0, 0
            def  c2,  0, 0, 0, 0
            dcl vPos.x
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x2 r0.xy, vPos.x, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m3x2 vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0, 0, 0
            def  c2,  0, 20, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x2 r0.xy, vPos.x, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : m3x2 vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20, 0, 0, 0
            def  c2,  0, 0, 0, 0
            dcl vPos.y
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x2 r0.xy, vPos.y, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m3x2 vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0, 0, 0
            def  c2,  0, 20, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x2 r0.xy, vPos.y, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m3x2 vPos.xy, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0, 0, 0
            def  c2,  0, 20, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x2 r0.xy, vPos.xy, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m3x2 vPos.yx, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0, 0, 0
            def  c2,  0, 20, 0, 0
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x2 r0.xy, vPos.yx, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : m3x3 vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20, 0, 0, 0
            def  c2,  0, 0, 0, 0
            def  c3,  0, 0, 0, 0
            dcl vPos.x
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x3 r0.xyz, vPos.x, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m3x3 vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0, 0
            def  c2,  0, 20,  0, 0
            def  c3,  0,  0, 20, 0
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x3 r0.xyz, vPos.x, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : m3x3 vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20, 0, 0, 0
            def  c2,  0, 0, 0, 0
            def  c3,  0, 0, 0, 0
            dcl vPos.y
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x3 r0.xyz, vPos.y, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m3x3 vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0, 0
            def  c2,  0, 20,  0, 0
            def  c3,  0,  0, 20, 0
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x3 r0.xyz, vPos.y, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m3x3 vPos.xy, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0, 0
            def  c2,  0, 20,  0, 0
            def  c3,  0,  0, 20, 0
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x3 r0.xyz, vPos.xy, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m3x3 vPos.yx, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0, 0
            def  c2,  0, 20,  0, 0
            def  c3,  0,  0, 20, 0
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x3 r0.xyz, vPos.yx, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : m3x4 vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0,  0
            def  c2,  0,  0,  0,  0
            def  c3,  0,  0, 20,  0
            def  c4,  0, 20,  0,  0
            dcl vPos.x
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x4 r0, vPos.x, c1
            mov r0.y, r0.w
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m3x4 vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0,  0
            def  c2,  0, 20,  0,  0
            def  c3,  0,  0,  0,  0
            def  c4,  0,  0, 20,  0
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x4 r0, vPos.x, c1
            mov r0.z, r0.w
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : m3x4 vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0,  0
            def  c2,  0,  0,  0,  0
            def  c3,  0,  0, 20,  0
            def  c4,  0, 20,  0,  0
            dcl vPos.y
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x4 r0, vPos.y, c1
            mov r0.y, r0.w
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m3x4 vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0,  0
            def  c2,  0, 20,  0,  0
            def  c3,  0,  0,  0,  0
            def  c4,  0,  0, 20,  0
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x4 r0, vPos.y, c1
            mov r0.z, r0.w
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m3x4 vPos.xy, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0,  0
            def  c2,  0,  0,  0,  0
            def  c3,  0,  0, 20,  0
            def  c4,  0, 20,  0,  0
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x4 r0, vPos.xy, c1
            mov r0.y, r0.w
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m3x4 vPos.yx, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0,  0
            def  c2,  0, 20,  0,  0
            def  c3,  0,  0,  0,  0
            def  c4,  0,  0, 20,  0
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m3x4 r0, vPos.yx, c1
            mov r0.z, r0.w
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : m4x3 vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0,  0, 0
            def  c1, 20,  0,  0, 0
            def  c2,  0, 20,  0, 0
            def  c3,  0,  0, 20, 0
            dcl vPos.x
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m4x3 r0.xyz, vPos.x, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m4x3 vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0,  0
            def  c2,  0, 20,  0,  0
            def  c3,  0,  0,  0, 20
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m4x3 r0.xyz, vPos.x, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : m4x3 vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0, 0
            def  c2,  0, 20,  0, 0
            def  c3,  0,  0, 20, 0
            dcl vPos.y
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m4x3 r0.xyz, vPos.y, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m4x3 vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0,  0
            def  c2,  0, 20,  0,  0
            def  c3,  0,  0,  0, 20
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m4x3 r0.xyz, vPos.y, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m4x3 vPos.xy, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0,  0
            def  c2,  0, 20,  0,  0
            def  c3,  0,  0, 20,  0
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m4x3 r0.xyz, vPos.xy, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m4x3 vPos.yx, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0, 0,  0
            def  c2,  0, 20, 0,  0
            def  c3,  0,  0, 0, 20
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m4x3 r0.xyz, vPos.yx, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : m4x4 vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0,  0
            def  c2,  0, 20,  0,  0
            def  c3,  0,  0, 20,  0
            def  c4,  0,  0,  0, 20
            dcl vPos.x
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m4x4 r0, vPos.x, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m4x4 vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0,  0
            def  c2,  0, 20,  0,  0
            def  c3,  0,  0, 20,  0
            def  c4,  0,  0,  0, 20
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m4x4 r0, vPos.x, c1
            mov r0.z, r0.w
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : m4x4 vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0,  0
            def  c2,  0, 20,  0,  0
            def  c3,  0,  0, 20,  0
            def  c4,  0,  0,  0, 20
            dcl vPos.y
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m4x4 r0, vPos.y, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m4x4 vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0,  0
            def  c2,  0, 20,  0,  0
            def  c3,  0,  0, 20,  0
            def  c4,  0,  0,  0, 20
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m4x4 r0, vPos.y, c1
            mov r0.z, r0.w
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m4x4 vPos.xy, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0,  0
            def  c2,  0, 20,  0,  0
            def  c3,  0,  0, 20,  0
            def  c4,  0,  0,  0, 20
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m4x4 r0, vPos.xy, c1
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : m4x4 vPos.yx, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            def  c0, 256, 0, 0, 0
            def  c1, 20,  0,  0,  0
            def  c2,  0, 20,  0,  0
            def  c3,  0,  0, 20,  0
            def  c4,  0,  0,  0, 20
            dcl vPos.xy
            
            mov r0, c0.z
            rcp r1.x, c0.x
            m4x4 r0, vPos.yx, c1
            mov r0.z, r0.w
            mul r0, r0, r1.x
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : mad X, X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.x
            
            mad r0, v0.x, c0.y, vPos.x
            rcp r1, c0.x
            mul r2, r1, r0
            frc r2, r2
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mad X, X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.xy
            
            mad r0, v0.x, c0.y, vPos.x
            rcp r1, c0.x
            mul r2, r1, r0
            frc r2, r2
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : mad X, vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.x
            
            mul r3, v0.x, c0.y
            rcp r1, c0.x
            mul r3, r3, r1
            mad r2, r1, vPos.x, r3 
            frc r2, r2
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mad X, vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.xy
            
            mul r3, v0.x, c0.y
            rcp r1, c0.x
            mul r3, r3, r1
            mad r2, r1, vPos.x, r3 
            frc r2, r2
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 vPos.x : mad vPos.x, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.x
            
            mul r3, v0.x, c0.y
            rcp r1, c0.x
            mul r3, r3, r1
            mad r2, vPos.x, r1, r3 
            frc r2, r2
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mad vPos.x, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.xy
            
            mul r3, v0.x, c0.y
            rcp r1, c0.x
            mul r3, r3, r1
            mad r2, vPos.x, r1, r3 
            frc r2, r2
            mov r2.yz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : mad X, X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.y
            
            mad r0, v0.y, c0.y, vPos.y
            rcp r1, c0.x
            mul r2, r1, r0
            frc r2, r2
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mad X, X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.xy
            
            mad r0, v0.y, c0.y, vPos.y
            rcp r1, c0.x
            mul r2, r1, r0
            frc r2, r2
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : mad X, vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.y
            
            mul r3, v0.y, c0.y
            rcp r1, c0.x
            mul r3, r3, r1
            mad r2, r1, vPos.y, r3 
            frc r2, r2
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mad X, vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.xy
            
            mul r3, v0.y, c0.y
            rcp r1, c0.x
            mul r3, r3, r1
            mad r2, r1, vPos.y, r3 
            frc r2, r2
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : mad vPos.y, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.y
            
            mul r3, v0.y, c0.y
            rcp r1, c0.x
            mul r3, r3, r1
            mad r2, vPos.y, r1, r3 
            frc r2, r2
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mad vPos.y, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.xy
            
            mul r3, v0.y, c0.y
            rcp r1, c0.x
            mul r3, r3, r1
            mad r2, vPos.y, r1, r3 
            frc r2, r2
            mov r2.xz, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mad X, X, vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.xy
            
            mad r0, v0.xy, c0.y, vPos.xy
            rcp r1, c0.x
            mul r2, r1, r0
            frc r2, r2
            mov r2.z, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mad X, vPos.xy, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.xy
            
            mul r3, v0.xy, c0.y
            rcp r1, c0.x
            mul r3, r3, r1
            mad r2, r1, vPos.xy, r3
            frc r2, r2
            mov r2.z, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mad vPos.xy, X, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {     
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_3_0
            dcl_texcoord v0.xy
            def c0, 256, 640, 0, 0
            dcl vPos.xy
            
            mul r3, v0.xy, c0.y
            rcp r1, c0.x
            mul r3, r3, r1
            mad r2, vPos.xy, r1, r3
            frc r2, r2
            mov r2.z, c0.z
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : max(min) vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.x
            
            mov r2.yz, c0.z
            mov r3.yz, c0.z
            rcp r1.x, c0.x
            max r2.xw, vPos.x, c1.x
            min r3.xw, vPos.x, c1.x
            sub r2, r2, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : max(min) vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.yz, c0.z
            mov r3.yz, c0.z
            rcp r1.x, c0.x
            max r2.xw, vPos.x, c1.z
            min r3.xw, vPos.x, c1.z
            sub r2, r2, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : max(min) vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.y
            
            mov r2.xz, c0.z
            mov r3.xz, c0.z
            rcp r1.x, c0.x
            max r2.yw, vPos.y, c1.w
            min r3.yw, vPos.y, c1.w
            sub r2, r2, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : max(min) vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.xz, c0.z
            mov r3.xz, c0.z
            rcp r1.x, c0.x
            max r2.yw, vPos.y, c1.y
            min r3.yw, vPos.y, c1.y
            sub r2, r2, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : max(min) vPos.xy, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.z, c0.z
            mov r3.z, c0.z
            rcp r1.x, c0.x
            max r2.xyw, vPos.xy, c1.xy
            min r3.xyw, vPos.xy, c1.xy
            sub r2, r2, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : max(min) vPos.xy, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.z, c0.z
            mov r3.z, c0.z
            rcp r1.x, c0.x
            max r2.xyw, vPos.xy, c1.zw
            min r3.xyw, vPos.xy, c1.zw
            sub r2, r2, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}



Technique 
< 
	String	Name = "ps_3_0 vPos.x : max(min) X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.x
            
            mov r2.yz, c0.z
            mov r3.yz, c0.z
            rcp r1.x, c0.x
            max r2.xw, c1.z, vPos.x
            min r3.xw, c1.z, vPos.x
            sub r2, r2, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : max(min) X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.yz, c0.z
            mov r3.yz, c0.z
            rcp r1.x, c0.x
            max r2.xw, c1.x, vPos.x
            min r3.xw, c1.x, vPos.x
            sub r2, r2, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : max(min) X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.y
            
            mov r2.xz, c0.z
            mov r3.xz, c0.z
            rcp r1.x, c0.x
            max r2.yw, c1.y, vPos.y
            min r3.yw, c1.y, vPos.y
            sub r2, r2, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : max(min) X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.xz, c0.z
            mov r3.xz, c0.z
            rcp r1.x, c0.x
            max r2.yw, c1.w, vPos.y
            min r3.yw, c1.w, vPos.y
            sub r2, r2, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : max(min) X, vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.z, c0.z
            mov r3.z, c0.z
            rcp r1.x, c0.x
            max r2.xyw, vPos.xy, c1.zy
            min r3.xyw, vPos.xy, c1.zy
            sub r2, r2, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : max(min) X, vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.z, c0.z
            mov r3.z, c0.z
            rcp r1.x, c0.x
            max r2.xyw, vPos.xy, c1.xw
            min r3.xyw, vPos.xy, c1.xw
            sub r2, r2, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : mov vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.x
            
            mov r2.yz, c0.z
            rcp r1.x, c0.x
            mov r2.xw, vPos.x
            sub r2.xw, r2, c1.x
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mov vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.yz, c0.z
            rcp r1.x, c0.x
            mov r2.xw, vPos.x
            sub r2.xw, c1.z, r2
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : mov vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.y
            
            mov r2.xz, c0.z
            rcp r1.x, c0.x
            mov r2.yw, vPos.y
            sub r2.yw, c1.w, r2
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mov vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.xz, c0.z
            rcp r1.x, c0.x
            mov r2.yw, vPos.y
            sub r2.yw, r2, c1.y
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mov vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 20, 0, 0
            dcl vPos.xy
            
            mov r2.z, c0.z
            rcp r1.x, c0.x
            mov r2.xyw, vPos.xy
            sub r2.xyw, r2, c1.xy
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : mul X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.x
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            mul r0, r1.xzzx, c1
            mul r2, r1.xzzx, vPos.x
            sub r0, r2, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mul X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            mul r0, r1.xzzx, c1
            mul r2, r1.xzzx, vPos.x
            sub r0, r2, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : mul vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.x
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            mul r0, r1.xzzx, c1
            mul r2, vPos.x, r1.xzzx
            sub r0, r2, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mul vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            mul r0, r1.xzzx, c1
            mul r2, vPos.x, r1.xzzx
            sub r0, r2, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : mul X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.y
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            mul r0, r1.zxzx, c1
            mul r2, r1.zxzx, vPos.y
            sub r0, r2, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mul X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            mul r0, r1.zxzx, c1
            mul r2, r1.zxzx, vPos.y
            sub r0, r2, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : mul vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.y
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            mul r0, r1.zxzx, c1
            mul r2, vPos.y, r1.zxzx
            sub r0, r2, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mul vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            mul r0, r1.zxzx, c1
            mul r2, vPos.y, r1.zxzx
            sub r0, r2, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mul X, vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            mul r0, r1.xxzx, c1
            mul r2, r1.xxzx, vPos.xy
            sub r0, r2, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : mul vPos.xy, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            mul r0, r1.xxzx, c1
            mul r2, vPos.xy, r1.xxzx
            sub r0, r2, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : pow vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, -1, 0, 0.0001
            def c2, 0, 1, 2, 3
            dcl vPos.x
            //apply pow instruction
			pow r0, vPos.x, c2.x
			pow r1, vPos.x, c2.y
			pow r2, vPos.x, c2.z
			pow r3, vPos.x, c2.w
			//check if pow works correctly
			if_gt r2.x, r3.x       //pow(x,2) <= pow(x,3)
				mov r4, c0.y
				texkill r4
			endif
			
			// calculate pow with mul, won't pass most of the time and move on to exp version test
			mov r4, c2.y
			if_eq r0.x, r4.x
				if_eq r1.x, vPos.x
					mul r4, vPos.x, vPos.x
					if_eq r2.x, r4.x
						mul r4, vPos.x, vPos.x
						mul r4, r4, vPos.x
						if_eq r3.x, r4.x
							call l0
							mov r4, c0.y
						endif
					endif
				endif
			endif
			if_ne r4.x, c0.y
				call l1
			endif
			ret
			
			label l1
			//check pow(x, 0) = 1
			log r4, vPos.x
			mul r4, r4, c2.x
			exp r4, r4.x
			sub r0, r0, r4      
			//check pow(x, 1) = x
			log r4, vPos.x
			mul r4, r4, c2.y
			exp r4, r4.x
			sub r1, r1, r4  
			//check pow(x, 2)
			log r4, vPos.x
			mul r4, r4, c2.z
			exp r4, r4.x
			sub r2, r2, r4
			//check pow(x, 3)
			log r4, vPos.x
			mul r4, r4, c2.w
			exp r4, r4.x
			sub r3, r3, r4        
			//let tolerance of c0.w = 0.0001
			sub r0, c0.w, r0_abs
			sub r1, c0.w, r1_abs
			sub r2, c0.w, r2_abs
			sub r3, c0.w, r3_abs
			mov r4, c0.z
			cmp r4.xw, r0, vPos.x, c0.y
			texkill r4
			cmp r4.xw, r1, vPos.x, c0.y
			texkill r4
			cmp r4.xw, r2, vPos.x, c0.y
			texkill r4
			cmp r4.xw, r3, vPos.x, c0.y
			texkill r4
			call l0
			ret
			
			label l0
			rcp r0, c0.x
			mul r4, c2.w, r0.x
			frc r2, r4
			mov oC0, r2
			ret
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : pow vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, -1, 0, 0.0001
            def c2, 0, 1, 2, 3
            dcl vPos.xy
            //apply pow instruction
			pow r0, vPos.x, c2.x
			pow r1, vPos.x, c2.y
			pow r2, vPos.x, c2.z
			pow r3, vPos.x, c2.w
			//check if pow works correctly
			if_gt r2.x, r3.x       //pow(x,2) <= pow(x,3)
				mov r4, c0.y
				texkill r4
			endif
			
			// calculate pow with mul, won't pass most of the time and move on to exp version test
			mov r4, c2.y
			if_eq r0.x, r4.x
				if_eq r1.x, vPos.x
					mul r4, vPos.x, vPos.x
					if_eq r2.x, r4.x
						mul r4, vPos.x, vPos.x
						mul r4, r4, vPos.x
						if_eq r3.x, r4.x
							call l0
							mov r4, c0.y
						endif
					endif
				endif
			endif
			if_ne r4.x, c0.y
				call l1
			endif
			ret
			
			label l1
			//check pow(x, 0) = 1
			log r4, vPos.x
			mul r4, r4, c2.x
			exp r4, r4.x
			sub r0, r0, r4      
			//check pow(x, 1) = x
			log r4, vPos.x
			mul r4, r4, c2.y
			exp r4, r4.x
			sub r1, r1, r4  
			//check pow(x, 2)
			log r4, vPos.x
			mul r4, r4, c2.z
			exp r4, r4.x
			sub r2, r2, r4
			//check pow(x, 3)
			log r4, vPos.x
			mul r4, r4, c2.w
			exp r4, r4.x
			sub r3, r3, r4        
			//let tolerance of c0.w = 0.0001
			sub r0, c0.w, r0_abs
			sub r1, c0.w, r1_abs
			sub r2, c0.w, r2_abs
			sub r3, c0.w, r3_abs
			mov r4, c0.z
			cmp r4.xw, r0, vPos.x, c0.y
			texkill r4
			cmp r4.xw, r1, vPos.x, c0.y
			texkill r4
			cmp r4.xw, r2, vPos.x, c0.y
			texkill r4
			cmp r4.xw, r3, vPos.x, c0.y
			texkill r4
			call l0
			ret
			
			label l0
			rcp r0, c0.x
			mul r4, c2.w, r0.x
			frc r2, r4
			mov oC0, r2
			ret
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : pow vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, -1, 0, 0.0001
            def c2, 0, 1, 2, 3
            dcl vPos.y
            //apply pow instruction
			pow r0, vPos.y, c2.x
			pow r1, vPos.y, c2.y
			pow r2, vPos.y, c2.z
			pow r3, vPos.y, c2.w
			//check if pow works correctly
			if_gt r2.y, r3.y       //pow(x,2) <= pow(x,3)
				mov r4, c0.y
				texkill r4
			endif

			// calculate pow with mul, won't pass most of the time and move on to exp version test
			mov r4, c2.y
			if_eq r0.x, r4.x
				if_eq r1.x, vPos.y
					mul r4, vPos.y, vPos.y
					if_eq r2.x, r4.x
						mul r4, vPos.y, vPos.y
						mul r4, r4, vPos.y
						if_eq r3.x, r4.x
							call l0
							mov r4, c0.y
						endif
					endif
				endif
			endif
			if_ne r4.x, c0.y
				call l1
			endif
			ret
			
			label l1
			//check pow(x, 0) = 1
			log r4, vPos.y
			mul r4, r4, c2.x
			exp r4, r4.x
			sub r0, r0, r4      
			//check pow(x, 1) = x
			log r4, vPos.y
			mul r4, r4, c2.y
			exp r4, r4.x
			sub r1, r1, r4  
			//check pow(x, 2)
			log r4, vPos.y
			mul r4, r4, c2.z
			exp r4, r4.x
			sub r2, r2, r4
			//check pow(x, 3)
			log r4, vPos.y
			mul r4, r4, c2.w
			exp r4, r4.x
			sub r3, r3, r4        
			//let tolerance of c0.w = 0.0001
			sub r0, c0.w, r0_abs
			sub r1, c0.w, r1_abs
			sub r2, c0.w, r2_abs
			sub r3, c0.w, r3_abs
			mov r4, c0.z
			cmp r4.xw, r0, vPos.y, c0.y
			texkill r4
			cmp r4.xw, r1, vPos.y, c0.y
			texkill r4
			cmp r4.xw, r2, vPos.y, c0.y
			texkill r4
			cmp r4.xw, r3, vPos.y, c0.y
			texkill r4
			call l0
			ret
			
			label l0
			rcp r0, c0.x
			mul r4, c2.w, r0.x
			frc r2, r4
			mov oC0, r2
			ret
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : pow vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, -1, 0, 0.0001
            def c2, 0, 1, 2, 3
            dcl vPos.xy
            //apply pow instruction
			pow r0, vPos.y, c2.x
			pow r1, vPos.y, c2.y
			pow r2, vPos.y, c2.z
			pow r3, vPos.y, c2.w
			//check if pow works correctly
			if_gt r2.y, r3.y       //pow(x,2) <= pow(x,3)
				mov r4, c0.y
				texkill r4
			endif
			// calculate pow with mul, won't pass most of the time and move on to exp version test
			mov r4, c2.y
			if_eq r0.x, r4.x
				if_eq r1.x, vPos.y
					mul r4, vPos.y, vPos.y
					if_eq r2.x, r4.x
						mul r4, vPos.y, vPos.y
						mul r4, r4, vPos.y
						if_eq r3.x, r4.x
							call l0
							mov r4, c0.y
						endif
					endif
				endif
			endif
			if_ne r4.x, c0.y
				call l1
			endif
			ret
			
			label l1
			//check pow(x, 0) = 1
			log r4, vPos.y
			mul r4, r4, c2.x
			exp r4, r4.x
			sub r0, r0, r4      
			//check pow(x, 1) = x
			log r4, vPos.y
			mul r4, r4, c2.y
			exp r4, r4.x
			sub r1, r1, r4  
			//check pow(x, 2)
			log r4, vPos.y
			mul r4, r4, c2.z
			exp r4, r4.x
			sub r2, r2, r4
			//check pow(x, 3)
			log r4, vPos.y
			mul r4, r4, c2.w
			exp r4, r4.x
			sub r3, r3, r4        
			//let tolerance of c0.w = 0.0001
			sub r0, c0.w, r0_abs
			sub r1, c0.w, r1_abs
			sub r2, c0.w, r2_abs
			sub r3, c0.w, r3_abs
			mov r4, c0.z
			cmp r4.xw, r0, vPos.y, c0.y
			texkill r4
			cmp r4.xw, r1, vPos.y, c0.y
			texkill r4
			cmp r4.xw, r2, vPos.y, c0.y
			texkill r4
			cmp r4.xw, r3, vPos.y, c0.y
			texkill r4
			call l0
			ret
			
			label l0
			rcp r0, c0.x
			mul r4, c2.w, r0.x
			frc r2, r4
			mov oC0, r2
			ret
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : rcp vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 500000, 1, 0
            dcl vPos.x
            
			rcp r3, vPos.x
            rcp r4, c1.z
            mov r2.yz, c0.w
            rcp r1.x, c0.x
            sub r2.xw, r3, r4
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
			mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : rcp vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 500000, 1, 0
            dcl vPos.xy
            
			rcp r3, vPos.x
            rcp r4, c1.x
            mov r2.yz, c0.w
            rcp r1.x, c0.x
            sub r2.xw, r4, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
			mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : rcp vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 500000, 1, 0
            dcl vPos.y
            
			rcp r3, vPos.y
            rcp r4, c1.y
            mov r2.xz, c0.w
            rcp r1.x, c0.x
            sub r2.yw, r4, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
			mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : rcp vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 500000, 1, 0
            dcl vPos.xy
            
			rcp r3, vPos.y
            rcp r4, c1.w
            mov r2.xz, c0.w
            rcp r1.x, c0.x
            sub r2.yw, r3, r4
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
			mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "ps_3_0 vPos.x : rsq vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 100000, 1, 0
            dcl vPos.x
            
			rsq r3, vPos.x
            rsq r4, c1.x
            mov r2.yz, c0.w
            rcp r1.x, c0.x
            sub r2.xw, r4, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
			mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : rsq vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 100000, 1, 0
            dcl vPos.xy
            
			rsq r3, vPos.x
            rsq r4, c1.z
            mov r2.yz, c0.w
            rcp r1.x, c0.x
            sub r2.xw, r3, r4
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
			mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : rsq vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 100000, 1, 0
            dcl vPos.y
            
			rsq r3, vPos.y
            rsq r4, c1.w
            mov r2.xz, c0.w
            rcp r1.x, c0.x
            sub r2.yw, r3, r4
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
			mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : rsq vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 256, 100000, 1, 0
            dcl vPos.xy
            
			rsq r3, vPos.y
            rsq r4, c1.y
            mov r2.xz, c0.w
            rcp r1.x, c0.x
            sub r2.yw, r4, r3
            mul r2, r2, c0.y
            mul r2, r2, r1.x
            frc r2, r2
			mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : sub X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.x
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            sub r0, c1.zw, vPos.x
            mul r0, r1.xzzx, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : sub X, vPos.x";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            sub r0, c1.zw, vPos.x
            mul r0, r1.xzzx, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.x : sub vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.x
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            sub r0, vPos.x, c1
            mul r0, r1.xzzx, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : sub vPos.x, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            sub r0, vPos.x, c1
            mul r0, r1.xzzx, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : sub X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.y
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            sub r0, c1.zw, vPos.y
            mul r0, r1.zxzx, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : sub X, vPos.y";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            sub r0, c1.zw, vPos.y
            mul r0, r1.zxzx, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.y : sub vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.y
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            sub r0, vPos.y, c1
            mul r0, r1.zxzx, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : sub vPos.y, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            sub r0, vPos.y, c1
            mul r0, r1.zxzx, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : sub X, vPos.xy";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            sub r0, c1.zw, vPos.xy
            mul r0, r1.xxzx, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 vPos.xy : sub vPos.xy, X";
	String	Shape = "TexTL";
>
{
    Pass P0 {
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = <fvPosLimits>;
		       
        PixelShader = asm {
            ps_3_0
            def c0, 12.8, -1, 0, 0
            dcl vPos.xy
            
            mov r1, c0
            rcp r1.x, c0.x
            //(vPos.x-Xmin, vPos.y-Ymin)%12.8
            sub r0, vPos.xy, c1
            mul r0, r1.xxzx, r0
            frc r2, r0
            mov oC0, r2
        };
         
        VertexShader = <TexTLVS>;
   }
}
