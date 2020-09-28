// Set emacs mode: -*-c++-*-
#include "include.fx"
		
Technique 
< 
	String	Name = "ifp ne test, output: yellow";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0, 0, 0.8, 0.8, 0
            def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            
            mov r0, c0
            mov r1, c1
            mov r2, c5

            setp_ne p0, r0, r1
            if p0.x
            add r2, r2, c2
            else
            add r2, r2, c5
            endif
        
            if !p0.y
            add r2, r2, c3
            else
            add r2, r2, c5
            endif
        
            if !p0.z
            add r2, r2, c4
            else
            add r2, r2, c5
            endif
        
            if !p0.w
            add r2, r2, c5
            else
            mov r2, c5
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "ifp eq test, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0, 0, 0.8, 0.8, 0
            def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            
            mov r0, c0
            mov r1, c1
            mov r2, c5

            setp_eq p0, r0, r1
            if p0.x
            add r2, r2, c2
            else
            add r2, r2, c5
            endif
        
            if p0.y
            add r2, r2, c3
            else
            add r2, r2, c5
            endif
        
            if !p0.z
            add r2, r2, c4
            else
            add r2, r2, c5
            endif
        
            if p0.w
            add r2, r2, c5
            else
            mov r2, c5
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "ifp ge test, output: green";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0, 0, 0.8, 0.8, 0
            def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            
            mov r0, c0
            mov r1, c1
            mov r2, c5

            setp_ge p0, r0, r1
            if p0.x
            add r2, r2, c2
            else
            add r2, r2, c5
            endif
        
            if p0.y
            add r2, r2, c3
            else
            add r2, r2, c5
            endif
        
            if !p0.z
            add r2, r2, c4
            else
            add r2, r2, c5
            endif
        
            if p0.w
            add r2, r2, c5
            else
            mov r2, c5
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "ifp le test, output: grey";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0, 0, 0.8, 0.8, 0
            def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            
            mov r0, c0
            mov r1, c1
            mov r2, c5

            setp_le p0, r0, r1
            if p0.x
            add r2, r2, c2
            else
            add r2, r2, c5
            endif
        
            if p0.y
            add r2, r2, c3
            else
            add r2, r2, c5
            endif
        
            if !p0.z
            add r2, r2, c4
            else
            add r2, r2, c5
            endif
        
            if p0.w
            add r2, r2, c5
            else
            mov r2, c5
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "ifp lt test, output: grey";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0, 0, 0.8, 0.8, 0
            def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            
            mov r0, c0
            mov r1, c1
            mov r2, c5

            setp_lt p0, r0, r1
            if p0.x
            add r2, r2, c2
            else
            add r2, r2, c5
            endif
        
            if !p0.y
            add r2, r2, c3
            else
            add r2, r2, c5
            endif
        
            if !p0.z
            add r2, r2, c4
            else
            add r2, r2, c5
            endif
        
            if !p0.w
            add r2, r2, c5
            else
            mov r2, c5
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "ifp gt test, output: green";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0, 0, 0.8, 0.8, 0
            def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            
            mov r0, c0
            mov r1, c1
            mov r2, c5

            setp_gt p0, r0, r1
            if p0.x
            add r2, r2, c2
            else
            add r2, r2, c5
            endif
        
            if !p0.y
            add r2, r2, c3
            else
            add r2, r2, c5
            endif
        
            if !p0.z
            add r2, r2, c4
            else
            add r2, r2, c5
            endif
        
            if !p0.w
            add r2, r2, c5
            else
            mov r2, c5
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "ifp test, inst count=256, output: green";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0, 0, 0.8, 0.8, 0
            def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            
            mov r0, c0
            mov r1, c1
            mov r2, c5

            setp_gt p0, r0, r1
            if p0.x 
                setp_ne p0, r0, r1
                if p0.x
                add r2, r2, c2
                else
                add r2, r2, c5
                endif
        
                if !p0.y
                add r2, r2, c3
                else
                add r2, r2, c5
                endif
        
                if !p0.z
                add r2, r2, c4
                else
                add r2, r2, c5
                endif
        
                if !p0.w
                add r2, r2, c5
                else
                mov r2, c5
                endif
                mov r2, c5
                add r2, r2, c2
                setp_gt p0, r0, r1
            else
            add r2, r2, c5
            endif
        
            if !p0.y
            add r2, r2, c3
            else
            add r2, r2, c5
            endif
        
            if !p0.z
            add r2, r2, c4
            else
            add r2, r2, c5
            endif
        
            if !p0.w
            add r2, r2, c5
            else
            mov r2, c5
            endif

            mov r0, c0
            mov r1, c1
            mov r2, c5

            setp_gt p0, r0, r1
            if p0.x 
                setp_ne p0, r0, r1
                if p0.x
                add r2, r2, c2
                else
                add r2, r2, c5
                endif
        
                if !p0.y
                add r2, r2, c3
                else
                add r2, r2, c5
                endif
        
                if !p0.z
                add r2, r2, c4
                else
                add r2, r2, c5
                endif
        
                if !p0.w
                add r2, r2, c5
                else
                mov r2, c5
                endif
                mov r2, c5
                add r2, r2, c2
                setp_gt p0, r0, r1
            else
            add r2, r2, c5
            endif
        
            if !p0.y
            add r2, r2, c3
            else
            add r2, r2, c5
            endif
        
            if !p0.z
            add r2, r2, c4
            else
            add r2, r2, c5
            endif
        
            if !p0.w
            add r2, r2, c5
            else
            mov r2, c5
            endif
            mov r0, c0
            mov r1, c1
            mov r2, c5

            setp_gt p0, r0, r1
            if p0.x 
                setp_ne p0, r0, r1
                if p0.x
                add r2, r2, c2
                else
                add r2, r2, c5
                endif
        
                if !p0.y
                add r2, r2, c3
                else
                add r2, r2, c5
                endif
        
                if !p0.z
                add r2, r2, c4
                else
                add r2, r2, c5
                endif
        
                if !p0.w
                add r2, r2, c5
                else
                mov r2, c5
                endif
                mov r2, c5
                add r2, r2, c2
                setp_gt p0, r0, r1
            else
            add r2, r2, c5
            endif
        
            if !p0.y
            add r2, r2, c3
            else
            add r2, r2, c5
            endif
        
            if !p0.z
            add r2, r2, c4
            else
            add r2, r2, c5
            endif
        
            if !p0.w
            add r2, r2, c5
            else
            mov r2, c5
            endif
            mov r0, c0
            mov r1, c1
            mov r2, c5

            setp_gt p0, r0, r1
            if p0.x 
                setp_ne p0, r0, r1
                if p0.x
                add r2, r2, c2
                else
                add r2, r2, c5
                endif
        
                if !p0.y
                add r2, r2, c3
                else
                add r2, r2, c5
                endif
        
                if !p0.z
                add r2, r2, c4
                else
                add r2, r2, c5
                endif
        
                if !p0.w
                add r2, r2, c5
                else
                mov r2, c5
                endif
                mov r2, c5
                add r2, r2, c2
                setp_gt p0, r0, r1
            else
            add r2, r2, c5
            endif
        
            if !p0.y
            add r2, r2, c3
            else
            add r2, r2, c5
            endif
        
            if !p0.z
            add r2, r2, c4
            else
            add r2, r2, c5
            endif
        
            if !p0.w
            add r2, r2, c5
            else
            mov r2, c5
            endif
            setp_ne p0, r0, r1
            setp_gt p0, r0, r1
            setp_ne p0, r0, r1
            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "ifp test, inst count=512, output: green";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0, 0, 0.8, 0.8, 0
            def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            
            mov r0, c0
            mov r1, c1
            mov r2, c5

            setp_gt p0, r0, r1
            if p0.x 
                setp_ne p0, r0, r1
                if p0.x
                add r2, r2, c2
                else
                add r2, r2, c5
                endif
        
                if !p0.y
                add r2, r2, c3
                else
                add r2, r2, c5
                endif
        
                if !p0.z
                add r2, r2, c4
                else
                add r2, r2, c5
                endif
        
                if !p0.w
                add r2, r2, c5
                else
                mov r2, c5
                endif
                mov r2, c5
                add r2, r2, c2
                setp_gt p0, r0, r1
            else
                add r2, r2, c5
            endif
        
            if !p0.y 
                setp_eq p0, r0, r1
                if p0.x
                add r2, r2, c2
                else
                add r2, r2, c5
                endif
        
                if p0.y
                add r2, r2, c3
                else
                add r2, r2, c5
                endif
        
                if !p0.z
                add r2, r2, c4
                else
                add r2, r2, c5
                endif
        
                if p0.w
                add r2, r2, c5
                else
                mov r2, c5
                endif

                setp_gt p0, r0, r1
                mov r2, c5
                add r2, r2, c2
                add r2, r2, c3

            else
            add r2, r2, c5
            endif
        
            if !p0.z 
                setp_ge p0, r0, r1
                if p0.x
                add r2, r2, c2
                else
                add r2, r2, c5
                endif
        
                if p0.y
                add r2, r2, c3
                else
                add r2, r2, c5
                endif
        
                if !p0.z
                add r2, r2, c4
                else
                add r2, r2, c5
                endif
        
                if p0.w
                add r2, r2, c5
                else
                mov r2, c5
                endif

                setp_gt p0, r0, r1
                mov r2, c5
                add r2, r2, c2
                add r2, r2, c3
                add r2, r2, c4
            else
            add r2, r2, c5
            endif
        
            if !p0.w 
                setp_le p0, r0, r1
                if p0.x
                add r2, r2, c2
                else
                add r2, r2, c5
                endif
        
                if p0.y
                add r2, r2, c3
                else
                add r2, r2, c5
                endif
        
                if !p0.z
                add r2, r2, c4
                else
                add r2, r2, c5
                endif
        
                if p0.w
                add r2, r2, c5
                else
                mov r2, c5
                endif
                setp_gt p0, r0, r1
                mov r2, c5
                add r2, r2, c2
                add r2, r2, c3
                add r2, r2, c4
                add r2, r2, c5
            else
            mov r2, c5
            endif

            setp_gt p0, r0, r1
            if p0.x 
                setp_ne p0, r0, r1
                if p0.x
                add r2, r2, c2
                else
                add r2, r2, c5
                endif
        
                if !p0.y
                add r2, r2, c3
                else
                add r2, r2, c5
                endif
        
                if !p0.z
                add r2, r2, c4
                else
                add r2, r2, c5
                endif
        
                if !p0.w
                add r2, r2, c5
                else
                mov r2, c5
                endif
                setp_gt p0, r0, r1
            else
                add r2, r2, c5
            endif
        
            if !p0.y 
                setp_eq p0, r0, r1
                if p0.x
                add r2, r2, c2
                else
                add r2, r2, c5
                endif
        
                if p0.y
                add r2, r2, c3
                else
                add r2, r2, c5
                endif
        
                if !p0.z
                add r2, r2, c4
                else
                add r2, r2, c5
                endif
        
                if p0.w
                add r2, r2, c5
                else
                mov r2, c5
                endif

                setp_gt p0, r0, r1
                mov r2, c5
                add r2, r2, c2
                add r2, r2, c3

            else
            add r2, r2, c5
            endif
        
            if !p0.z 
                setp_ge p0, r0, r1
                if p0.x
                add r2, r2, c2
                else
                add r2, r2, c5
                endif
        
                if p0.y
                add r2, r2, c3
                else
                add r2, r2, c5
                endif
        
                if !p0.z
                add r2, r2, c4
                else
                add r2, r2, c5
                endif
        
                if p0.w
                add r2, r2, c5
                else
                mov r2, c5
                endif

                setp_gt p0, r0, r1
                mov r2, c5
                add r2, r2, c2
                add r2, r2, c3
                add r2, r2, c4
            else
            add r2, r2, c5
            endif
        
            if !p0.w 
                setp_le p0, r0, r1
                if p0.x
                add r2, r2, c2
                else
                add r2, r2, c5
                endif
        
                if p0.y
                add r2, r2, c3
                else
                add r2, r2, c5
                endif
        
                if !p0.z
                add r2, r2, c4
                else
                add r2, r2, c5
                endif
        
                if p0.w
                add r2, r2, c5
                else
                mov r2, c5
                endif
                setp_gt p0, r0, r1
                mov r2, c5
                add r2, r2, c2
                add r2, r2, c3
                add r2, r2, c4
                add r2, r2, c5
            else
            mov r2, c5
            endif

            mov r0, c0
            mov r1, c1
            mov r2, c5
            setp_gt p0, r0, r1
            if p0.x 
                setp_ne p0, r0, r1
                if p0.x
                add r2, r2, c2
                else
                add r2, r2, c5
                endif
        
                if !p0.y
                add r2, r2, c3
                else
                add r2, r2, c5
                endif
        
                if !p0.z
                add r2, r2, c4
                else
                add r2, r2, c5
                endif
        
                if !p0.w
                add r2, r2, c5
                else
                mov r2, c5
                endif
                mov r2, c5
                add r2, r2, c2
                setp_gt p0, r0, r1
            else
            add r2, r2, c5
            endif
        
            if !p0.y
            add r2, r2, c3
            else
            add r2, r2, c5
            endif
        
            if !p0.z
            add r2, r2, c4
            else
            add r2, r2, c5
            endif
        
            if !p0.w
            add r2, r2, c5
            else
            mov r2, c5
            endif
            mov r0, c0
            mov r1, c1
            mov r2, c5

            setp_gt p0, r0, r1
            if p0.x 
                setp_ne p0, r0, r1
                if p0.x
                add r2, r2, c2
                else
                add r2, r2, c5
                endif
        
                if !p0.y
                add r2, r2, c3
                else
                add r2, r2, c5
                endif
        
                if !p0.z
                add r2, r2, c4
                else
                add r2, r2, c5
                endif
        
                if !p0.w
                add r2, r2, c5
                else
                mov r2, c5
                endif
                mov r2, c5
                add r2, r2, c2
                setp_gt p0, r0, r1
            else
            add r2, r2, c5
            endif
        
            if !p0.y
            add r2, r2, c3
            else
            add r2, r2, c5
            endif
        
            if !p0.z
            add r2, r2, c4
            else
            add r2, r2, c5
            endif
        
            if !p0.w
            add r2, r2, c5
            else
            mov r2, c5
            endif
            mov r0, c0
            mov r1, c1
            mov r2, c5

            setp_gt p0, r0, r1
            if p0.x 
                setp_ne p0, r0, r1
                if p0.x
                add r2, r2, c2
                else
                add r2, r2, c5
                endif
        
                if !p0.y
                add r2, r2, c3
                else
                add r2, r2, c5
                endif
        
                if !p0.z
                add r2, r2, c4
                else
                add r2, r2, c5
                endif
        
                if !p0.w
                add r2, r2, c5
                else
                mov r2, c5
                endif
                mov r2, c5
                add r2, r2, c2
                setp_gt p0, r0, r1
            else
            add r2, r2, c5
            endif
        
            if !p0.y
            add r2, r2, c3
            else
            add r2, r2, c5
            endif
        
            if !p0.z
            add r2, r2, c4
            else
            add r2, r2, c5
            endif
        
            if !p0.w
            add r2, r2, c5
            else
            mov r2, c5
            endif
            setp_ne p0, r0, r1
            setp_gt p0, r0, r1
            setp_ne p0, r0, r1
            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}
