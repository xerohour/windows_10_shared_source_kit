// Set emacs mode: -*-c++-*-
#include "include.fx"
		
Technique 
< 
	String	Name = "if bool  test: static if depth=1, inst count=128, output: cyan";
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
            def c6, 0.8, 0.8, 0.8, 0.8
            def c7, 1, 1, 1, 1
            defb b0, true
            defb b1, false
            defb b2, true
            defb b3, false
            defb b4, true
            defb b5, false
            defb b6, true
            defb b7, false
            defb b8, true
            defb b9, false
            defb b10, true
            defb b11, false
            defb b12, true
            defb b13, false
            defb b14, true
            defb b15, false

            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if b0
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            else
            add r2.x, -r3.xxxx, t0.xxxx
            endif
        
            if b1
            add r2.y, r2.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            endif
        

            if b2
            add r2, r2, c4            // add uniform blue
            else
            mov r2, c5                // black out image
            endif
        
            if b3
            mov r2, c5                // black out image
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif

            if b4
            add r2, r2, c5           // add black to image, i.e. noop
            else
            mov r2, c5                // black out image
            endif
        
            if b5
            mov r2, c5                // black out image
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif
        

            if b6
            add r2, r2, c5           // add black to image, i.e. noop
            else
            mov r2, c5                // black out image
            endif
        
            if b7
            mov r2, c5                // black out image
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif

            if b8
            add r2, r2, c5           // add black to image, i.e. noop
            else
            mov r2, c5                // black out image
            endif
        
            if b9
            mov r2, c5                // black out image
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif
        

            if b10
            add r2, r2, c5           // add black to image, i.e. noop
            else
            mov r2, c5                // black out image
            endif
        
            if b11
            mov r2, c5                // black out image
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif

            if b12
            add r2, r2, c5           // add black to image, i.e. noop
            else
            mov r2, c5                // black out image
            endif
        
            if b13
            mov r2, c5                // black out image
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif
        

            if b14
            add r2, r2, c5           // add black to image, i.e. noop
            else
            mov r2, c5                // black out image
            endif
        
            if b15
            mov r2, c5                // black out image
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif

            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if bool  test: static if depth=5, inst count=128, output: cyan";
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
            def c6, 0.8, 0.8, 0.8, 0.8
            def c7, 1, 1, 1, 1
            defb b0, true
            defb b1, false
            defb b2, true
            defb b3, false
            defb b4, true
            defb b5, false
            defb b6, true
            defb b7, false
            defb b8, true
            defb b9, false
            defb b10, true
            defb b11, false
            defb b12, true
            defb b13, false
            defb b14, true
            defb b15, false

            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if b0
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            else
            add r2.x, -r3.xxxx, t0.xxxx
            endif
        
            if b1
            add r2.y, r2.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            endif
        

            if b2
            add r2, r2, c4            // add uniform blue
            else
            mov r2, c5                // black out image
            endif
        
            if b3
            mov r2, c5                // black out image
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif

            if b4
            add r2, r2, c5           // add black to image, i.e. noop
            else
            mov r2, c5                // black out image
            endif
        
            if b5
            mov r2, c5                // black out image
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif
        

            if b6
            if b7
            mov r2, c5                // black out image
            else
            if b8
            if b9
            mov r2, c5                // black out image
            else
            if b10
            add r2, r2, c5           // add black to image, i.e. noop
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            endif
        
            if b11
            mov r2, c5                // black out image
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif

            if b12
            add r2, r2, c5           // add black to image, i.e. noop
            else
            mov r2, c5                // black out image
            endif
        
            if b13
            mov r2, c5                // black out image
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif
        

            if b14
            add r2, r2, c5           // add black to image, i.e. noop
            else
            mov r2, c5                // black out image
            endif
        
            if b15
            mov r2, c5                // black out image
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif

            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mul r6, r7, r6
            mul r6, r7, r6
            mul r6, r7, r6
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if bool  test: static if depth=24, inst count=145, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader = asm {
            ps_2_x
            def c0, 0.0, 1, .8, 1
            def c5, 0, 0, 0, 0
            defb b0, true
            defb b1, false
            defb b2, true
            defb b3, false
            defb b4, true
            defb b5, false
            defb b6, true
            defb b7, false
            defb b8, true
            defb b9, false
            defb b10, true
            defb b11, false
            defb b12, true
            defb b13, false
            defb b14, true
            defb b15, false

            if b6
            if b7
            mov oC0, c5                // black out image
            else
            if b8
            if b9
            mov oC0, c5                // black out image
            else
            if b10
            if b11
            mov oC0, c5                // black out image
            else
            if b12
            if b13
            mov oC0, c5                // black out image
            else
            if b14
            if b15
            mov oC0, c5                // black out image
            else
            if b5
            mov oC0, c5                // black out image
            else
            if b6
            if b7
            mov oC0, c5                // black out image
            else
            if b8
            if b9
            mov oC0, c5                // black out image
            else
            if b10
            if b11
            mov oC0, c5                // black out image
            else
            if b12
            if b13
            mov oC0, c5                // black out image
            else
            if b14
            if b15
            mov oC0, c5                // black out image
            else
            if b13
            mov oC0, c5                // black out image
            else
            if b14
            if b15
            mov oC0, c5                // black out image
            else
            mov oC0, c0                // set to expected output color
            endif
            else
            mov oC0, c5                // black out image
            endif
            endif
            endif
            else
            mov oC0, c5                // black out image
            endif
            endif
            else
            mov oC0, c5                // black out image
            endif
            endif
            else
            mov oC0, c5                // black out image
            endif
            endif
            else
            mov oC0, c5                // black out image
            endif
            endif
            else
            mov oC0, c5                // black out image
            endif
            endif
            endif
            else
            mov oC0, c5                // black out image
            endif
            endif
            else
            mov oC0, c5                // black out image
            endif
            endif
            else
            mov oC0, c5                // black out image
            endif
            endif
            else
            mov oC0, c5                // black out image
            endif
            endif
            else
            mov oC0, c5                // black out image
            endif
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if bool  test: static if depth=24, inst count=256, output: cyan";
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
            def c6, 0.8, 0.8, 0.8, 0.8
            def c7, 1, 1, 1, 1
            defb b0, true
            defb b1, false
            defb b2, true
            defb b3, false
            defb b4, true
            defb b5, false
            defb b6, true
            defb b7, false
            defb b8, true
            defb b9, false
            defb b10, true
            defb b11, false
            defb b12, true
            defb b13, false
            defb b14, true
            defb b15, false

            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mul r6, r7, r6
            mul r6, r7, r6
            mul r6, r7, r6
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5

            if b0
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            else
            add r2.x, -r3.xxxx, t0.xxxx
            endif
        
            if b1
            add r2.y, r2.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            endif
        

            if b2
            add r2, r2, c4            // add uniform blue
            else
            mov r2, c5                // black out image
            endif
        
            if b3
            mov r2, c5                // black out image
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif

            if b4
            add r2, r2, c5           // add black to image, i.e. noop
            else
            mov r2, c5                // black out image
            endif
        
            if b6
            if b7
            mov r2, c5                // black out image
            else
            if b8
            if b9
            mov r2, c5                // black out image
            else
            if b10
            if b11
            mov r2, c5                // black out image
            else
            if b12
            if b13
            mov r2, c5                // black out image
            else
            if b14
            if b15
            mov r2, c5                // black out image
            else
            if b5
            mov r2, c5                // black out image
            else
            if b6
            if b7
            mov r2, c5                // black out image
            else
            if b8
            if b9
            mov r2, c5                // black out image
            else
            if b10
            if b11
            mov r2, c5                // black out image
            else
            if b12
            if b13
            mov r2, c5                // black out image
            else
            if b14
            if b15
            mov r2, c5                // black out image
            else
            if b13
            mov r2, c5                // black out image
            else
            if b14
            if b15
            mov r2, c5                // black out image
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif
            else
            mov r2, c5                // black out image
            endif

            endif
            endif
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            endif
            endif
            endif
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
        
            endif
            endif
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
        
            endif
            endif
            else
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
        
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
        
            endif
            endif
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
        
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if bool  test: static if depth=24, inst count=512, output: cyan";
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
            def c6, 0.8, 0.8, 0.8, 0.8
            def c7, 1, 1, 1, 1
            defb b0, true
            defb b1, false
            defb b2, true
            defb b3, false
            defb b4, true
            defb b5, false
            defb b6, true
            defb b7, false
            defb b8, true
            defb b9, false
            defb b10, true
            defb b11, false
            defb b12, true
            defb b13, false
            defb b14, true
            defb b15, false

            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mul r6, r7, r6
            mul r6, r7, r6
            mul r6, r7, r6
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5

            if b0
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            else
            add r2.x, -r3.xxxx, t0.xxxx
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            endif
        
            if b1
            add r2.y, r2.y, r3.y
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            endif
        

            if b2
            add r2, r2, c4            // add uniform blue
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            endif
        
            if b3
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif

            if b4
            add r2, r2, c5           // add black to image, i.e. noop
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            endif
        
            if b6
            if b7
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            else
            if b8
            if b9
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            else
            if b10
            if b11
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            else
            if b12
            if b13
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            else
            if b14
            if b15
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            else
            if b5
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            else
            if b6
            if b7
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            else
            if b8
            if b9
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            else
            if b10
            if b11
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            else
            if b12
            if b13
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            else
            if b14
            if b15
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            else
            if b13
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            else
            if b14
            if b15
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            endif

            endif
            endif
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            endif
            endif
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            endif
            endif
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            endif
            endif
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            endif
            endif
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            endif
            endif
            endif
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            endif
            endif
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
        
            endif
            endif
            else
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
        
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            endif
            endif
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
        
            endif
            endif
            else
            mov r2, c5                // black out image
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            endif

            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
        
            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

#if 0
Technique 
< 
	String	Name = "if bool  test: static if depth > 24, inst count=???, output: cyan";
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
            def c6, 0.8, 0.8, 0.8, 0.8
            def c7, 1, 1, 1, 1
            defb b0, true
            defb b1, false
            defb b2, true
            defb b3, false
            defb b4, true
            defb b5, false
            defb b6, true
            defb b7, false
            defb b8, true
            defb b9, false
            defb b10, true
            defb b11, false
            defb b12, true
            defb b13, false
            defb b14, true
            defb b15, false

            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if b0
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            else
            add r2.x, -r3.xxxx, t0.xxxx
            endif
        
            if b1
            add r2.y, r2.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            endif
        

            if b2
            add r2, r2, c4            // add uniform blue
            else
            mov r2, c5                // black out image
            endif
        
            if b3
            mov r2, c5                // black out image
            else
            add r2, r2, c5           // add black to image, i.e. noop
            endif

            if b4
            add r2, r2, c5           // add black to image, i.e. noop
            else
            mov r2, c5                // black out image
            endif
        
            if b6
            if b7
            mov r2, c5                // black out image
            else
            if b8
            if b9
            mov r2, c5                // black out image
            else
            if b10
            if b11
            mov r2, c5                // black out image
            else
            if b12
            if b13
            mov r2, c5                // black out image
            else
            if b14
            if b15
            mov r2, c5                // black out image
            else
            if b5
            mov r2, c5                // black out image
            else
            if b6
            if b7
            mov r2, c5                // black out image
            else
            if b8
            if b9
            mov r2, c5                // black out image
            else
            if b10
            if b11
            mov r2, c5                // black out image
            else
            if b12
            if b13
            mov r2, c5                // black out image
            else
            if b14
            if b15
            mov r2, c5                // black out image
            else
            if b13
            mov r2, c5                // black out image
            else
            if b14
            if b15
            mov r2, c5                // black out image
            else
            if b6
            if b7
            mov r2, c5                // black out image
            else
            if b8
            if b9
            mov r2, c5                // black out image
            else
            if b10
            add r2, r2, c5           // add black to image, i.e. noop
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            endif

            endif
            else
            mov r2, c5                // black out image
            endif

            endif
            endif
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            endif
            endif
            endif
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            endif
            endif
            else
            mov r2, c5                // black out image
            endif
        
            mov r5, t0.xy
            mov r6, t0.xy
            mul r7, r2, c7
            mul r6, r7, r6
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5
            mad r5, r5, c7, r2
            mad r6, r6, c7, r5

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}
#endif
