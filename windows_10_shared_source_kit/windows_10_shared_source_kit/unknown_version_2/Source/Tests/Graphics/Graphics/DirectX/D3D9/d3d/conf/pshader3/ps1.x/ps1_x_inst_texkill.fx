#include "include.fx"

Technique 
< 
	String	Name = "Inst: TexKill";
	String	Shape = "CubeTL0";
>
{
    Pass P0
    {        
        
        Texture[0] = <TexCube_0>;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader = asm
        {
            ps_1_1
            def         c1, 1.0, 1.0, 1.0, 1.0

            texkill     t0
            mov         r0, c1
        };
   }
}

Technique 
< 
	String	Name = "Inst: TexKill, U component";
	String	Shape = "Quad";
>
{
    Pass P0
    {        
        
        Texture[0] = <Tex2D_0>;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        
        VertexShader = 
        asm
        {
                        vs_1_1
                        dcl_position	v0
                        dcl_normal	v1
                        dcl_texcoord0	v2

                        def	c4,	-0.5,	0.0,	1.0,	1.0

                        m4x4	oPos,	v0,     c0
                        
                        add     r0, v2, c4

                        mov	oT0,	r0
                        mov     oD0,    r0
        };

		       
        PixelShader = asm
        {
            ps_1_1
            def         c1, 1.0, 1.0, 1.0, 1.0

            texkill     t0
            mov         r0, c1
        };
   }
}

Technique 
< 
	String	Name = "Inst: TexKill, V component";
	String	Shape = "Quad";
>
{
    Pass P0
    {
        Texture[0] = <Tex2D_0>;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        
        VertexShader = 
        asm
        {
                        vs_1_1
                        dcl_position	v0
                        dcl_normal	v1
                        dcl_texcoord0	v2

                        def	c4,	0.0,	-0.5,	1.0,	1.0

                        m4x4	oPos,	v0,     c0
                        
                        add     r0, v2, c4

                        mov	oT0,	r0
        };
		       
        PixelShader = asm
        {
            ps_1_1
            def         c1, 1.0, 1.0, 1.0, 1.0

            texkill     t0
            mov         r0, c1
        };
         
   }
}

Technique 
< 
	String	Name = "Reg: Temp, Inst: TexKill, UV component";
	String	Shape = "Quad";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        
        VertexShader = 
        asm
        {
                        vs_1_1
                        dcl_position	v0
                        dcl_normal	v1
                        dcl_texcoord0	v2

                        def	c4,	-0.5,	-0.5,	1.0,	1.0

                        m4x4	oPos,	v0,     c0
                        
                        add     r0, v2, c4
                        
                        mov	oT0,	r0
        };

		       
        PixelShader = asm
        {
            ps_1_1
            def         c1, 1.0, 1.0, 1.0, 1.0

            texkill     t0
            mov         r0, c1
        };
         
   }
}

