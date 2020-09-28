// Set emacs mode: -*-c++-*-
#include "include.fx"
		
Technique 
< 
	String	Name = "if_gt r(n), r(m) swizzle test: output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            ////def c0, 0, 0.8, 0.8, 0
            ////def c1, 0.8, 0.8, 0.0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if_gt r0.x, r1.y
            add r2.x, -r3.xxxx, t0.xxxx
            else
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            endif
        
            if_gt r0.w, r1.x
            mov r2, c5                // black out image
            else
            add r2, r2, c5               // add black to image, noop
            endif
        
            if_gt r0.w, r1.y
            mov r2, c5                // black out image
            else
            add r2, r2, c5               // add black to image, noop
            endif
        
            if_gt r0.y, r1.x
            mov r2, c5                // black out image
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            endif
        
            if_gt r0.z, r1.x
            mov r2, c5                // black out image
            else
            add r2, r2, c5               // add black to image, noop
            endif
        
            if_gt r0.z, r1.y
            mov r2, c5                // black out image
            else
            add r2, r2, c5               // add black to image, noop
            endif
        

            if_gt r0.y, r1.z
            add r2, r2, c4            // add uniform blue
            else
            mov r2, c5                // black out image
            endif
        
            if_gt r0.y, r1.w
            add r2, r2, c5               // add black to image, noop
            else
            mov r2, c5                // black out image
            endif

            if_gt r0.z, r1.w
            add r2, r2, c5               // add black to image, noop
            else
            mov r2, c5                // black out image
            endif

            //r0 = 0,    0.8, 0.8, 0
            //r1 = 0.8, 0.8, 0.0, 0
            // possible untested swizzles
            //xy, xz, xw, yx, yz, yw, zx, zy, zw, wx, wy, wz
            // x    w   w   y    z    z    y    y    z      x     x    w
            if_gt r0.w, r1.z
            mov r2, c5                // black out image if alpha channel test fails
            else
            add r2, r2, c5
            endif

            if_gt r0.x, r1.z
            mov r2, c5                // black out image if alpha channel test fails
            else
            add r2, r2, c5               // add black to image, noop
            endif

            if_gt r0.x, r1.w
            mov r2, c5                // black out image if alpha channel test fails
            else
            add r2, r2, c5               // add black to image, noop
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_lt r(n), r(m) swizzle test: output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if_lt r0.x, r1.y
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            else
            mov r2, c5                // black out image
            endif

            if_lt r0.w, r1.x
            add r2, r2, c5               // add black to image, noop
            else
            mov r2, c5                // black out image
            endif

            if_lt r0.w, r1.y
            add r2, r2, c5               // add black to image, noop
            else
            mov r2, c5                // black out image
            endif

        
            if_lt r0.y, r1.x
            mov r2, c5                // black out image
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            endif
            if_lt r0.z, r1.x
            mov r2, c5                // black out image
            else
            add r2, r2, c5               // add black to image, noop
            endif
            if_lt r0.z, r1.y
            mov r2, c5                // black out image
            else
            add r2, r2, c5               // add black to image, noop
            endif
        
            if_lt r0.y, r1.z
            mov r2, c5                // black out image
            else
            add r2, r2, c4            // add uniform blue
            endif
            if_lt r0.y, r1.w
            mov r2, c5                // black out image
            else
            add r2, r2, c5               // add black to image, noop
            endif
            if_lt r0.z, r1.w
            mov r2, c5                // black out image
            else
            add r2, r2, c5               // add black to image, noop
            endif
        
            //r0 = 0,    0.8, 0.8, 0
            //r1 = 0.8, 0.8, 0.0, 0
            // possible untested swizzles
            //xy, xz, xw, yx, yz, yw, zx, zy, zw, wx, wy, wz
            // x    w   w   y    z    z     y   y    z     x    x      w
            if_lt r0.w, r1.z
            mov r2, c5                // black out image if alpha channel test fails
            else
            add r2, r2, c5
            endif
            if_lt r0.x, r1.z
            mov r2, c5                // black out image if alpha channel test fails
            else
            add r2, r2, c5               // add black to image, noop
            endif
            if_lt r0.x, r1.w
            mov r2, c5                // black out image if alpha channel test fails
            else
            add r2, r2, c5               // add black to image, noop
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_{ne,eq,ge,le}  r(n), r(m) swizzle test #1: output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            def c8, 0.33333, 0.33333, 0.33333, 0.33333
            dcl t0.xy

            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r5, t0.xy
            mov r6, t0.xy
            mov r3, -t0.xy
            
            //r0 = 0,    0.8, 0.8, 0
            //r1 = 0.8, 0.8, 0.0, 0
            // possible untested swizzles
            //xy, xz, xw, yx, yz, yw, zx, zy, zw, wx, wy, wz
            // x    y    x     z    x    y    x    y   z     y     z     z
            if_ne r0.z, r1.w
              if_ne r0.y, r1.x
                mov r2, c5                // black out image
              else
                if_ne r0.w, r1.y
                  if_ne r0.w, r1.z
                    mov r2, c5                // black out image
                  else
                    add r2.x, r3.xxxx, t0.xxxx             // cancel out red
                  endif
                else
                  mov r2, c5                // black out image
                endif
              endif
            else
              mov r2, c5                // black out image
            endif

            //r0 = 0,    0.8, 0.8, 0
            //r1 = 0.8, 0.8, 0.0, 0
            // possible untested swizzles
            //xy, xz, xw, yx, yz, yw, zx, zy, zw, wx, wy, wz
            // x   x     x    y     x    y   z     z    y    z      z    y
            if_eq r0.w, r1.x
                mov r2, c5                // black out image
            else
                if_eq r0.z, r1.x
                    if_eq r0.w, r1.y
                        mov r2, c5                // black out image
                    else 
                        if_eq r0.z, r1.y
                            sub r4, c7.yyyy, t0.yyyy            // add 1-g to g channel
                            add r2.y, r2, r4
                        else 
                            mov r2, c5            // black out image
                        endif                    
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif

        
            //r0 = 0,    0.8, 0.8, 0
            //r1 = 0.8, 0.8, 0.0, 0
            // possible untested swizzles
            //xy, xz, xw, yx, yz, yw, zx, zy, zw, wx, wy, wz
            // x    x    x    x    y    y     z    z    z     y     z    y
            if_ge r0.w, r1.y
                mov r2, c5                // black out image
            else 
                if_ge r0.z, r1.x
                    if_ge r0.z, r1.y
                        if_ge r0.z, r1.w 
                           add r2, r2, c4 // blue
                        else 
                            mov r2, c5                // black out image if alpha channel test fails
                        endif
                    else 
                        mov r2, c5                // black out image
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif
        
            //r0 = 0,    0.8, 0.8, 0
            //r1 = 0.8, 0.8, 0.0, 0
            // possible untested swizzles
            //xy, xz, xw, yx, yz, yw, zx, zy, zw, wx, wy, wz
            // x    x    y    x    y    x    y    y    z     z     z     z
            if_le r0.w, r1.y
                if_le r0.w, r1.z
                    if_le r0.z, r1.w
                        mov r2, c5                // black out image
                    else 
                        if_le r0.w, r1.x
                            add r2, r2, c5               // add black to image, noop
                        else 
                            mov r2, c5                // black out image
                        endif
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            else 
                mov r2, c5                // black out image
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_{ne,eq,ge,le} r(n), r(m) swizzle test #2: output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            def c8, 0.33333, 0.33333, 0.33333, 0.33333
            dcl t0.xy

            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r5, t0.xy
            mov r6, t0.xy
            mov r3, -t0.xy
            
            //r0 = 0,    0.8, 0.8, 0
            //r1 = 0.8, 0.8, 0.0, 0
            // possible untested swizzles
            //xy, xz, xw, yx, yz, yw, zx, zy, zw, wx, wy, wz
            // x    y    x           x    y    x    y          y
            if_ne r0.y, r1.w
              if_ne r0.z, r1.y
                mov r2, c5                // black out image
              else
                if_ne r0.w, r1.x
                  if_ne r0.x, r1.z
                    mov r2, c5                // black out image
                  else
                    add r2.x, r3.xxxx, t0.xxxx             // cancel out red
                  endif
                else
                  mov r2, c5                // black out image
                endif
              endif
            else
              mov r2, c5                // black out image
            endif

            //r0 = 0,    0.8, 0.8, 0
            //r1 = 0.8, 0.8, 0.0, 0
            // possible untested swizzles
            //xy, xz, xw, yx, yz, yw, zx, zy, zw, wx, wy, wz
            // x   x     x    y     x    y               y                  y
            if_eq r0.y, r1.w 
                mov r2, c5                // black out image
            else
                if_eq r0.w, r1.z
                    if_eq r0.z, r1.w 
                        mov r2, c5                // black out image
                    else 
                        if_eq r0.y, r1.x
                            sub r4, c7.yyyy, t0.yyyy            // add 1-g to g channel
                            add r2.y, r2, r4
                        else 
                            mov r2, c5            // black out image
                        endif                    
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif

        
            //r0 = 0,    0.8, 0.8, 0
            //r1 = 0.8, 0.8, 0.0, 0
            // possible untested swizzles
            //xy, xz, xw, yx, yz, yw, zx, zy, zw, wx, wy, wz
            // x    x    x    x    y    y                       y           y
            if_ge r0.w, r1.x
                mov r2, c5                // black out image
            else 
                if_ge r0.w, r1.z 
                    if_ge r0.y, r1.z
                        if_ge r0.y, r1.w 
                           add r2, r2, c4 // blue
                        else 
                            mov r2, c5                // black out image if alpha channel test fails
                        endif
                    else 
                        mov r2, c5                // black out image
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif
        
            //r0 = 0,    0.8, 0.8, 0
            //r1 = 0.8, 0.8, 0.0, 0
            // possible untested swizzles
            //xy, xz, xw, yx, yz, yw, zx, zy, zw, wx, wy, wz
            // x    x    y    x     y     x   y    y
            if_le r0.z, r1.x
                if_le r0.z, r1.y
                    if_le r0.y, r1.z
                        mov r2, c5                // black out image
                    else 
                        if_le r0.x, r1.w
                            add r2, r2, c5               // add black to image, noop
                        else 
                            mov r2, c5                // black out image
                        endif
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            else 
                mov r2, c5                // black out image
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_{ne,eq,ge,le} r(n), r(m) swizzle test #3: output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            def c8, 0.33333, 0.33333, 0.33333, 0.33333
            dcl t0.xy

            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r5, t0.xy
            mov r6, t0.xy
            mov r3, -t0.xy
            
            //r0 = 0,    0.8, 0.8, 0
            //r1 = 0.8, 0.8, 0.0, 0
            // possible untested swizzles
            //xy, xz, xw, yx, yz, yw, zx, zy, zw, wx, wy, wz
            // x         x           x          x
            if_ne r0.x, r1.y
              if_ne r0.z, r1.x
                mov r2, c5                // black out image
              else
                if_ne r0.y, r1.z
                  if_ne r0.x, r1.w
                    mov r2, c5                // black out image
                  else
                    add r2.x, r3.xxxx, t0.xxxx             // cancel out red
                  endif
                else
                  mov r2, c5                // black out image
                endif
              endif
            else
              mov r2, c5                // black out image
            endif

            //r0 = 0,    0.8, 0.8, 0
            //r1 = 0.8, 0.8, 0.0, 0
            // possible untested swizzles
            //xy, xz, xw, yx, yz, yw, zx, zy, zw, wx, wy, wz
            // x   x     x          x
            if_eq r0.x, r1.y 
                mov r2, c5                // black out image
            else
                if_eq r0.x, r1.z
                    if_eq r0.y, r1.z 
                        mov r2, c5                // black out image
                    else 
                        if_eq r0.x, r1.w 
                            sub r4, c7.yyyy, t0.yyyy            // add 1-g to g channel
                            add r2.y, r2, r4
                        else 
                            mov r2, c5            // black out image
                        endif                    
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif

        
            //r0 = 0,    0.8, 0.8, 0
            //r1 = 0.8, 0.8, 0.0, 0
            // possible untested swizzles
            //xy, xz, xw, yx, yz, yw, zx, zy, zw, wx, wy, wz
            // x    x    x    x
            if_ge r0.x, r1.y
                mov r2, c5                // black out image
            else 
                if_ge r0.x, r1.z 
                    if_ge r0.x, r1.w 
                        if_ge r0.y, r1.x 
                           add r2, r2, c4 // blue
                        else 
                            mov r2, c5                // black out image if alpha channel test fails
                        endif
                    else 
                        mov r2, c5                // black out image
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif
        
            //r0 = 0,    0.8, 0.8, 0
            //r1 = 0.8, 0.8, 0.0, 0
            // possible untested swizzles
            //xy, xz, xw, yx, yz, yw, zx, zy, zw, wx, wy, wz
            // x    x         x           x
            if_le r0.x, r1.y 
                if_le r0.x, r1.z 
                    if_le r0.y, r1.w
                        mov r2, c5                // black out image
                    else 
                        if_le r0.y, r1.x 
                            add r2, r2, c5               // add black to image, noop
                        else 
                            mov r2, c5                // black out image
                        endif
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            else 
                mov r2, c5                // black out image
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "ifc  r(n), r(m) test: dynamic flow control depth=4, inst count=143, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            def c8, 0.33333, 0.33333, 0.33333, 0.33333
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r5, t0.xy
            mov r6, t0.xy
            mov r3, -t0.xy
            
            if_ne r0.x, r1.x
              if_ne r0.y, r1.y
                mov r2, c5                // black out image
              else
                if_ne r0.z, r1.z
                  if_ne r0.w, r1.w
                    mov r2, c5                // black out image
                  else
                    add r2.x, r3.xxxx, t0.xxxx             // cancel out red
                  endif
                else
                  mov r2, c5                // black out image
                endif
              endif
            else
              mov r2, c5                // black out image
            endif

            if_eq r0.x, r1.x 
                mov r2, c5                // black out image
            else
                if_eq r0.y, r1.y
                    if_eq r0.z, r1.z 
                        mov r2, c5                // black out image
                    else 
                        if_eq r0.w, r1.w 
                            sub r4, c7.yyyy, t0.yyyy            // add 1-g to g channel
                            add r2.y, r2, r4
                        else 
                            mov r2, c5            // black out image
                        endif                    
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif
        
        
            if_ge r0.x, r1.x 
                mov r2, c5                // black out image
            else 
                if_ge r0.y, r1.y 
                    if_ge r0.z, r1.z 
                        if_ge r0.w, r1.w 
                            add r2, r2, c4
                        else 
                            mov r2, c5                // black out image if alpha channel test fails
                        endif
                    else 
                        mov r2, c5                // black out image
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif
        
        
            if_le r0.x, r1.x 
                if_le r0.y, r1.y 
                    if_le r0.z, r1.z 
                        mov r2, c5                // black out image
                    else 
                        if_le r0.w, r1.w 
                            add r2, r2, c5               // add black to image, noop
                        else 
                            mov r2, c5                // black out image
                        endif
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            else 
                mov r2, c5                // black out image
            endif
        
            if_gt r0.x, r1.x
            add r5.x, -r3.xxxx, t0.xxxx
            else
            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
            endif
        
            if_gt r0.y, r1.y
            add r5.y, r5.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r5.y, r5, r4
            endif
        
            if_gt r0.z, r1.z
            add r5, r5, c4            // add uniform blue
            else
            mov r5, c5                // black out image
            endif
        
            if_gt r0.w, r1.w
            mov r5, c5                // black out image if alpha channel test fails
            else
            add r5, r5, c5
            endif

            mov r6, r5  
            mov r6, r5  
            mov r6, r5
            mul r2, r2, c8
            mad r5, r5, c8, r2
            mad r6, r6, c8, r5
            mov oC0, r6
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = " ifc  r(n), r(m) test: dynamic flow control depth=4, inst count=169, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            def c8, 0.33333, 0.33333, 0.33333, 0.33333
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r5, t0.xy
            mov r6, t0.xy
            mov r3, -t0.xy
            
            if_ne r0.x, r1.x
              if_ne r0.y, r1.y
                mov r2, c5                // black out image
              else
                if_ne r0.z, r1.z
                  if_ne r0.w, r1.w
                    mov r2, c5                // black out image
                  else
                    add r2.x, r3.xxxx, t0.xxxx             // cancel out red
                  endif
                else
                  mov r2, c5                // black out image
                endif
              endif
            else
              mov r2, c5                // black out image
            endif

            if_eq r0.x, r1.x 
                mov r2, c5                // black out image
            else
                if_eq r0.y, r1.y
                    if_eq r0.z, r1.z 
                        mov r2, c5                // black out image
                    else 
                        if_eq r0.w, r1.w 
                            sub r4, c7.yyyy, t0.yyyy            // add 1-g to g channel
                            add r2.y, r2, r4
                        else 
                            mov r2, c5            // black out image
                        endif                    
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif
        
        
            if_ge r0.x, r1.x 
                mov r2, c5                // black out image
            else 
                if_ge r0.y, r1.y 
                    if_ge r0.z, r1.z 
                        if_ge r0.w, r1.w 
                            add r2, r2, c4
                        else 
                            mov r2, c5                // black out image if alpha channel test fails
                        endif
                    else 
                        mov r2, c5                // black out image
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif
        
        
            if_le r0.x, r1.x 
                if_le r0.y, r1.y 
                    if_le r0.z, r1.z 
                        mov r2, c5                // black out image
                    else 
                        if_le r0.w, r1.w 
                            add r2, r2, c5               // add black to image, noop
                        else 
                            mov r2, c5                // black out image
                        endif
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            else 
                mov r2, c5                // black out image
            endif
        
            if_gt r0.x, r1.x
            add r5.x, -r3.xxxx, t0.xxxx
            else
            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
            endif
        
            if_gt r0.y, r1.y
            add r5.y, r5.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r5.y, r5, r4
            endif
        
            if_gt r0.z, r1.z
            add r5, r5, c4            // add uniform blue
            else
            mov r5, c5                // black out image
            endif
        
            if_gt r0.w, r1.w
            mov r5, c5                // black out image if alpha channel test fails
            else
            add r5, r5, c5
            endif

            if_lt r0.x, r1.x
            add r6.x, r3.xxxx, t0.xxxx             // cancel out red
            else
            add r6.x, -r3.xxxx, t0.xxxx
            endif
        
            if_lt r0.y, r1.y
            add r6.y, r6.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r6.y, r6, r4
            endif
        
            if_lt r0.z, r1.z
            mov r6, c5                // black out image
            else
            add r6, r6, c4            // add uniform blue
            endif
        
            if_lt r0.w, r1.w
            mov r6, c5                // black out image if alpha channel test fails
            else
            add r6, r6, c5
            endif

            mul r2, r2, c8
            mad r5, r5, c8, r2
            mad r6, r6, c8, r5
            mov oC0, r6
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = " if_ne r(n), r(m) test: dynamic flow control depth=1, inst count=34, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if_ne r0.x, r1.x
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            else
            add r2.x, -r3.xxxx, t0.xxxx
            endif
        
            if_ne r0.y, r1.y
            add r2.y, r2.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            endif
        

            if_ne r0.z, r1.z
            add r2, r2, c4            // add uniform blue
            else
            mov r2, c5                // black out image
            endif
        
            if_ne r0.w, r1.w
            mov r2, c5                // black out image if alpha channel test fails
            else
            add r2, r2, c5
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_eq r(n), r(m) test: dynamic flow control depth=1, inst count=34, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if_eq r0.x, r1.x
            add r2.x, -r3.xxxx, t0.xxxx
            else
            add r2.x, r3.xxxx, t0.xxxx            // cancel out red
            endif
        
            if_eq r0.y, r1.y
            sub r4, c7.yyyy, t0.yyyy            // add 1-g to g channel
            add r2.y, r2, r4
            else
            add r2.y, r2.y, r3.y
            endif
        
            if_eq r0.z, r1.z
            mov r2, c5                // black out image
            else
            add r2, r2, c4            // add uniform blue
            endif
        
            if_eq r0.w, r1.w
            add r2, r2, c5
            else
            mov r2, c5            // black out image if alpha channel test fails
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_ge r(n), r(m) test: dynamic flow control depth=1, inst count=34, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if_ge r0.x, r1.x
            add r2.x, -r3.xxxx, t0.xxxx
            else
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            endif
        
            if_ge r0.y, r1.y
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            else
            add r2.y, r2.y, r3.y
            endif
        
            if_ge r0.z, r1.z
            add r2, r2, c4            // add uniform blue
            else
            mov r2, c5                // black out image
            endif
        
            if_ge r0.w, r1.w
            add r2, r2, c5
            else
            mov r2, c5                // black out image if alpha channel test fails
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_le r(n), r(m) test: dynamic flow control depth=1, inst count=34, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if_le r0.x, r1.x
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            else
            add r2.x, -r3.xxxx, t0.xxxx
            endif
        
            if_le r0.y, r1.y
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            else
            add r2.y, r2.y, r3.y
            endif
        
            if_le r0.z, r1.z
            mov r2, c5                // black out image
            else
            add r2, r2, c4            // add uniform blue
            endif
        
            if_le r0.w, r1.w
            add r2, r2, c5
            else
            mov r2, c5                // black out image if alpha channel test fails
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_gt r(n), r(m) test: dynamic flow control depth=1, inst count=34, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if_gt r0.x, r1.x
            add r2.x, -r3.xxxx, t0.xxxx
            else
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            endif
        
            if_gt r0.y, r1.y
            add r2.y, r2.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            endif
        
            if_gt r0.z, r1.z
            add r2, r2, c4            // add uniform blue
            else
            mov r2, c5                // black out image
            endif
        
            if_gt r0.w, r1.w
            mov r2, c5                // black out image if alpha channel test fails
            else
            add r2, r2, c5
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_lt r(n), r(m) test: dynamic flow control depth=1, inst count=34, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if_lt r0.x, r1.x
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            else
            add r2.x, -r3.xxxx, t0.xxxx
            endif
        
            if_lt r0.y, r1.y
            add r2.y, r2.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            endif
        
            if_lt r0.z, r1.z
            mov r2, c5                // black out image
            else
            add r2, r2, c4            // add uniform blue
            endif
        
            if_lt r0.w, r1.w
            mov r2, c5                // black out image if alpha channel test fails
            else
            add r2, r2, c5
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_ne c(n), c(n) test: dynamic flow control depth=1, inst count=34, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if_ne c0.x, c0.y
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            else
            add r2.x, -r3.xxxx, t0.xxxx
            endif
        
            if_ne c0.y, c0.z
            add r2.y, r2.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            endif
        

            if_ne c0.z, c0.w
            add r2, r2, c4            // add uniform blue
            else
            mov r2, c5                // black out image
            endif
        
            if_ne c0.w, c0.x
            mov r2, c5                // black out image if alpha channel test fails
            else
            add r2, r2, c5
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_eq c(n), c(n) test: dynamic flow control depth=1, inst count=34, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if_eq c0.x, c0.y
            add r2.x, -r3.xxxx, t0.xxxx
            else
            add r2.x, r3.xxxx, t0.xxxx            // cancel out red
            endif
        
            if_eq c0.y, c0.z
            sub r4, c7.yyyy, t0.yyyy            // add 1-g to g channel
            add r2.y, r2, r4
            else
            add r2.y, r2.y, r3.y
            endif
        
            if_eq c0.z, c0.w
            mov r2, c5                // black out image
            else
            add r2, r2, c4            // add uniform blue
            endif
        
            if_eq c0.w, c0.x
            add r2, r2, c5
            else
            mov r2, c5            // black out image if alpha channel test fails
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_ge c(n), c(n) test: dynamic flow control depth=1, inst count=34, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if_ge c0.x, c0.y
            add r2.x, -r3.xxxx, t0.xxxx
            else
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            endif
        
            if_ge c0.y, c0.z
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            else
            add r2.y, r2.y, r3.y
            endif
        
            if_ge c0.z, c0.w
            add r2, r2, c4            // add uniform blue
            else
            mov r2, c5                // black out image
            endif
        
            if_ge c0.w, c0.x
            add r2, r2, c5
            else
            mov r2, c5                // black out image if alpha channel test fails
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_le c(n), c(n) test: dynamic flow control depth=1, inst count=34, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if_le c0.x, c0.y
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            else
            add r2.x, -r3.xxxx, t0.xxxx
            endif
        
            if_le c0.y, c0.z
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            else
            add r2.y, r2.y, r3.y
            endif
        
            if_le c0.z, c0.w
            mov r2, c5                // black out image
            else
            add r2, r2, c4            // add uniform blue
            endif
        
            if_le c0.w, c0.x
            add r2, r2, c5
            else
            mov r2, c5                // black out image if alpha channel test fails
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_gt c(n), c(n) test: dynamic flow control depth=1, inst count=34, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if_gt c0.x, c0.y
            add r2.x, -r3.xxxx, t0.xxxx
            else
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            endif
        
            if_gt c0.y, c0.z
            add r2.y, r2.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            endif
        
            if_gt c0.z, c0.w
            add r2, r2, c4            // add uniform blue
            else
            mov r2, c5                // black out image
            endif
        
            if_gt c0.w, c0.x
            mov r2, c5                // black out image if alpha channel test fails
            else
            add r2, r2, c5
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_lt c(n), c(n) test: dynamic flow control depth=1, inst count=34, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r3, -t0.xy
            
            if_lt c0.x, c0.y
            add r2.x, r3.xxxx, t0.xxxx             // cancel out red
            else
            add r2.x, -r3.xxxx, t0.xxxx
            endif
        
            if_lt c0.y, c0.z
            add r2.y, r2.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r2.y, r2, r4
            endif
        
            if_lt c0.z, c0.w
            mov r2, c5                // black out image
            else
            add r2, r2, c4            // add uniform blue
            endif
        
            if_lt c0.w, c0.x
            mov r2, c5                // black out image if alpha channel test fails
            else
            add r2, r2, c5
            endif

            mov oC0, r2
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_* c(n), c(n) test: dynamic flow control depth=4, inst count=143, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            def c8, 0.33333, 0.33333, 0.33333, 0.33333
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r5, t0.xy
            mov r6, t0.xy
            mov r3, -t0.xy
            
            if_ne c0.x, c0.y
              if_ne c0.y, c0.z
                mov r2, c5                // black out image
              else
                if_ne c0.z, c0.w
                  if_ne c0.w, c0.x
                    mov r2, c5                // black out image
                  else
                    add r2.x, r3.xxxx, t0.xxxx             // cancel out red
                  endif
                else
                  mov r2, c5                // black out image
                endif
              endif
            else
              mov r2, c5                // black out image
            endif

            if_eq c0.x, c0.y 
                mov r2, c5                // black out image
            else
                if_eq c0.y, c0.z
                    if_eq c0.z, c0.w 
                        mov r2, c5                // black out image
                    else 
                        if_eq c0.w, c0.x 
                            sub r4, c7.yyyy, t0.yyyy            // add 1-g to g channel
                            add r2.y, r2, r4
                        else 
                            mov r2, c5            // black out image
                        endif                    
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif
        
        
            if_ge c0.x, c0.y 
                mov r2, c5                // black out image
            else 
                if_ge c0.y, c0.z 
                    if_ge c0.z, c0.w 
                        if_ge c0.w, c0.x 
                            add r2, r2, c4
                        else 
                            mov r2, c5                // black out image if alpha channel test fails
                        endif
                    else 
                        mov r2, c5                // black out image
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif
        
        
            if_le c0.x, c0.y 
                if_le c0.y, c0.z 
                    if_le c0.z, c0.w 
                        mov r2, c5                // black out image
                    else 
                        if_le c0.w, c0.x 
                            add r2, r2, c5               // add black to image, noop
                        else 
                            mov r2, c5                // black out image
                        endif
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            else 
                mov r2, c5                // black out image
            endif
        
            if_gt c0.x, c0.y
            add r5.x, -r3.xxxx, t0.xxxx
            else
            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
            endif
        
            if_gt c0.y, c0.z
            add r5.y, r5.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r5.y, r5, r4
            endif
        
            if_gt c0.z, c0.w
            add r5, r5, c4            // add uniform blue
            else
            mov r5, c5                // black out image
            endif
        
            if_gt c0.w, c0.x
            mov r5, c5                // black out image if alpha channel test fails
            else
            add r5, r5, c5
            endif

            mov r6, r5  
            mov r6, r5  
            mov r6, r5
            mul r2, r2, c8
            mad r5, r5, c8, r2
            mad r6, r6, c8, r5
            mov oC0, r6
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_* c(n), c(n) test: dynamic flow control depth=4, inst count=169, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            def c8, 0.33333, 0.33333, 0.33333, 0.33333
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r5, t0.xy
            mov r6, t0.xy
            mov r3, -t0.xy
            
            if_ne c0.x, c0.y
              if_ne c0.y, c0.z
                mov r2, c5                // black out image
              else
                if_ne c0.z, c0.w
                  if_ne c0.w, c0.x
                    mov r2, c5                // black out image
                  else
                    add r2.x, r3.xxxx, t0.xxxx             // cancel out red
                  endif
                else
                  mov r2, c5                // black out image
                endif
              endif
            else
              mov r2, c5                // black out image
            endif

            if_eq c0.x, c0.y 
                mov r2, c5                // black out image
            else
                if_eq c0.y, c0.z
                    if_eq c0.z, c0.w 
                        mov r2, c5                // black out image
                    else 
                        if_eq c0.w, c0.x 
                            sub r4, c7.yyyy, t0.yyyy            // add 1-g to g channel
                            add r2.y, r2, r4
                        else 
                            mov r2, c5            // black out image
                        endif                    
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif
        
        
            if_ge c0.x, c0.y 
                mov r2, c5                // black out image
            else 
                if_ge c0.y, c0.z 
                    if_ge c0.z, c0.w 
                        if_ge c0.w, c0.x 
                            add r2, r2, c4
                        else 
                            mov r2, c5                // black out image if alpha channel test fails
                        endif
                    else 
                        mov r2, c5                // black out image
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif
        
        
            if_le c0.x, c0.y 
                if_le c0.y, c0.z 
                    if_le c0.z, c0.w 
                        mov r2, c5                // black out image
                    else 
                        if_le c0.w, c0.x 
                            add r2, r2, c5               // add black to image, noop
                        else 
                            mov r2, c5                // black out image
                        endif
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            else 
                mov r2, c5                // black out image
            endif
        
            if_gt c0.x, c0.y
            add r5.x, -r3.xxxx, t0.xxxx
            else
            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
            endif
        
            if_gt c0.y, c0.z
            add r5.y, r5.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r5.y, r5, r4
            endif
        
            if_gt c0.z, c0.w
            add r5, r5, c4            // add uniform blue
            else
            mov r5, c5                // black out image
            endif
        
            if_gt c0.w, c0.x
            mov r5, c5                // black out image if alpha channel test fails
            else
            add r5, r5, c5
            endif

            if_lt c0.x, c0.y
            add r6.x, r3.xxxx, t0.xxxx             // cancel out red
            else
            add r6.x, -r3.xxxx, t0.xxxx
            endif
        
            if_lt c0.y, c0.z
            add r6.y, r6.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r6.y, r6, r4
            endif
        
            if_lt c0.z, c0.w
            mov r6, c5                // black out image
            else
            add r6, r6, c4            // add uniform blue
            endif
        
            if_lt c0.w, c0.x
            mov r6, c5                // black out image if alpha channel test fails
            else
            add r6, r6, c5
            endif

            mul r2, r2, c8
            mad r5, r5, c8, r2
            mad r6, r6, c8, r5
            mov oC0, r6
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_* c(n), c(n) test: dynamic flow control depth=4, inst count=256, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            def c8, 0.33333, 0.33333, 0.33333, 0.33333
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
            mov r5, t0.xy
            mov r6, t0.xy
            mov r3, -t0.xy
            
            if_ne c0.x, c0.y
              if_ne c0.y, c0.z
                mov r2, c5                // black out image
              else
                if_ne c0.z, c0.w
                  if_ne c0.w, c0.x
                    mov r2, c5                // black out image
                  else 
                    if b0
                    add r2, r2, c5           // add black to image, i.e. noop
                    else
                    mov r2, c5                // black out image
                    endif
        
                    if b1
                    mov r2, c5                // black out image
                    else
                    add r2, r2, c5           // add black to image, i.e. noop
                    endif

                    if b2
                    add r2, r2, c5           // add black to image, i.e. noop
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

                    add r2.x, r3.xxxx, t0.xxxx             // cancel out red
                    endif
                else
                  mov r2, c5                // black out image
                endif
              endif
            else
              mov r2, c5                // black out image
            endif

            if_eq c0.x, c0.y 
                mov r2, c5                // black out image
            else
                if_eq c0.y, c0.z
                    if_eq c0.z, c0.w 
                        mov r2, c5                // black out image
                    else 
                        if_eq c0.w, c0.x 
                            sub r4, c7.yyyy, t0.yyyy            // add 1-g to g channel
                            add r2.y, r2, r4
                        else 
                            mov r2, c5            // black out image
                        endif                    
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif
        
        
            if_ge c0.x, c0.y 
                mov r2, c5                // black out image
            else 
                if_ge c0.y, c0.z 
                    if_ge c0.z, c0.w 
                        if_ge c0.w, c0.x 
                            add r2, r2, c4
                        else 
                            mov r2, c5                // black out image if alpha channel test fails
                        endif
                    else 
                        mov r2, c5                // black out image
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif
        
        
            if_le c0.x, c0.y 
                if_le c0.y, c0.z 
                    if_le c0.z, c0.w 
                        mov r2, c5                // black out image
                    else 
                        if_le c0.w, c0.x 
                            add r2, r2, c5               // add black to image, noop
                        else 
                            mov r2, c5                // black out image
                        endif
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            else 
                mov r2, c5                // black out image
            endif
        
            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r5.y, r5, r4
            add r5, r5, c4            // add uniform blue

            if_lt c0.x, c0.y
            add r6.x, r3.xxxx, t0.xxxx             // cancel out red
            else
            add r6.x, -r3.xxxx, t0.xxxx
            endif
        
            if_lt c0.y, c0.z
            add r6.y, r6.y, r3.y
            else
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r6.y, r6, r4
            endif
        
            if_lt c0.z, c0.w
            mov r6, c5                // black out image
            else
            add r6, r6, c4            // add uniform blue
            endif
        
            if_lt c0.w, c0.x
            mov r6, c5                // black out image if alpha channel test fails
            else
            add r6, r6, c5
            endif

            mul r2, r2, c8
            mad r5, r5, c8, r2
            mad r6, r6, c8, r5
            mov oC0, r6
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "if_* c(n), c(n) test: dynamic flow control depth=5, inst count=512, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;        
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_2_x
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0, 0, 0
            def c3, 0, 0.8, 0, 0
            def c4, 0, 0, 0.8, 0
            def c5, 0, 0, 0, 0
            def c7, 1, 1, 1, 1
            def c8, 0.33333, 0.33333, 0.33333, 0.33333
            dcl t0.xy
            mov r0, c0
            mov r1, c1
            mov r2, t0.xy
            mov r5, t0.xy
            mov r6, t0.xy
            mov r3, -t0.xy
            
            if_ne c0.x, c0.y
              if_ne c0.y, c0.z
                mov r2, c5                // black out image
              else
                if_ne c0.z, c0.w
                  if_ne c0.w, c0.x
                    mov r2, c5                // black out image
            else 
                add r2.x, r3.xxxx, t0.xxxx             // cancel out red
                mov r5, t0.xy
                mov r6, t0.xy
                add r5.x, r3.xxxx, t0.xxxx             // cancel out red
                sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                add r5.y, r5, r4
                add r5, r5, c4            // add uniform blue

                add r6.x, r3.xxxx, t0.xxxx             // cancel out red
                sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                add r6.y, r6, r4
                add r6, r6, c4            // add uniform blue
                add r6, r6, c5

                endif
                else
                  mov r2, c5                // black out image
                endif
              endif
            else
              mov r2, c5                // black out image
            endif

            if_eq c0.x, c0.y 
                mov r2, c5                // black out image
            else
                if_eq c0.y, c0.z
                    if_eq c0.z, c0.w 
                        mov r2, c5                // black out image
                    else 
                        if_eq c0.w, c0.x 
                            sub r4, c7.yyyy, t0.yyyy            // add 1-g to g channel
                            add r2.y, r2, r4
                        else 
                            mov r2, c5            // black out image
                            if_gt c0.x, c0.y
                            add r5.x, -r3.xxxx, t0.xxxx
                            else
                            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
                            endif
        
                            if_gt c0.y, c0.z
                            add r5.y, r5.y, r3.y
                            else
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r5.y, r5, r4
                            endif
        
                            if_gt c0.z, c0.w
                            add r5, r5, c4            // add uniform blue
                            else
                            mov r5, c5                // black out image
                            endif
        
                            if_gt c0.w, c0.x
                            mov r5, c5                // black out image if alpha channel test fails
                            else
                            add r5, r5, c5
                            endif

                            if_lt c0.x, c0.y
                            add r6.x, r3.xxxx, t0.xxxx             // cancel out red
                            else
                            add r6.x, -r3.xxxx, t0.xxxx
                            endif
        
                            if_lt c0.y, c0.z
                            add r6.y, r6.y, r3.y
                            else
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r6.y, r6, r4
                            endif
        
                            if_lt c0.z, c0.w
                            mov r6, c5                // black out image
                            else
                            add r6, r6, c4            // add uniform blue
                            endif
        
                            if_lt c0.w, c0.x
                            mov r6, c5                // black out image if alpha channel test fails
                            else
                            add r6, r6, c5
                            endif
                        endif                    
                    endif
                else 
                    mov r2, c5                // black out image
                endif
            endif
        
        
            if_ge c0.x, c0.y 
                mov r2, c5                // black out image
            else 
                if_ge c0.y, c0.z 
                    if_ge c0.z, c0.w 
                        if_ge c0.w, c0.x 
                            add r2, r2, c4
                        else 
                            mov r2, c5                // black out image if alpha channel test fails
                        endif
                    else 
                        mov r2, c5                // black out image
                            if_gt c0.x, c0.y
                            add r5.x, -r3.xxxx, t0.xxxx
                            else
                            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
                            endif
        
                            if_gt c0.y, c0.z
                            add r5.y, r5.y, r3.y
                            else
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r5.y, r5, r4
                            endif
        
                            if_gt c0.z, c0.w
                            add r5, r5, c4            // add uniform blue
                            else
                            mov r5, c5                // black out image
                            endif
        
                            if_gt c0.w, c0.x
                            mov r5, c5                // black out image if alpha channel test fails
                            else
                            add r5, r5, c5
                            endif

                            if_lt c0.x, c0.y
                            add r6.x, r3.xxxx, t0.xxxx             // cancel out red
                            else
                            add r6.x, -r3.xxxx, t0.xxxx
                            endif
        
                            if_lt c0.y, c0.z
                            add r6.y, r6.y, r3.y
                            else
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r6.y, r6, r4
                            endif
        
                            if_lt c0.z, c0.w
                            mov r6, c5                // black out image
                            else
                            add r6, r6, c4            // add uniform blue
                            endif
        
                            if_lt c0.w, c0.x
                            mov r6, c5                // black out image if alpha channel test fails
                            else
                            add r6, r6, c5
                            endif
                    endif
                else 
                    mov r2, c5                // black out image
                            if_gt c0.x, c0.y
                            add r5.x, -r3.xxxx, t0.xxxx
                            else
                            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
                            endif
        
                            if_gt c0.y, c0.z
                            add r5.y, r5.y, r3.y
                            else
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r5.y, r5, r4
                            endif
        
                            if_gt c0.z, c0.w
                            add r5, r5, c4            // add uniform blue
                            else
                            mov r5, c5                // black out image
                            endif
        
                            if_gt c0.w, c0.x
                            mov r5, c5                // black out image if alpha channel test fails
                            else
                            add r5, r5, c5
                            endif

                            if_lt c0.x, c0.y
                            add r6.x, r3.xxxx, t0.xxxx             // cancel out red
                            else
                            add r6.x, -r3.xxxx, t0.xxxx
                            endif
        
                            if_lt c0.y, c0.z
                            add r6.y, r6.y, r3.y
                            else
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r6.y, r6, r4
                            endif
        
                            if_lt c0.z, c0.w
                            mov r6, c5                // black out image
                            else
                            add r6, r6, c4            // add uniform blue
                            endif
        
                            if_lt c0.w, c0.x
                            mov r6, c5                // black out image if alpha channel test fails
                            else
                            add r6, r6, c5
                            endif
                endif
            endif
        
        
            if_le c0.x, c0.y 
                if_le c0.y, c0.z 
                    if_le c0.z, c0.w 
                        mov r2, c5                // black out image
                    else 
                        if_le c0.w, c0.x 
                            add r2, r2, c5               // add black to image, noop
                        else 
                            mov r2, c5                // black out image
                            if_gt c0.x, c0.y
                            add r5.x, -r3.xxxx, t0.xxxx
                            else
                            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
                            endif
        
                            if_gt c0.y, c0.z
                            add r5.y, r5.y, r3.y
                            else
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r5.y, r5, r4
                            endif
        
                            if_gt c0.z, c0.w
                            add r5, r5, c4            // add uniform blue
                            else
                            mov r5, c5                // black out image
                            endif
        
                            if_gt c0.w, c0.x
                            mov r5, c5                // black out image if alpha channel test fails
                            else
                            add r5, r5, c5
                            endif

                            if_lt c0.x, c0.y
                            add r6.x, r3.xxxx, t0.xxxx             // cancel out red
                            else
                            add r6.x, -r3.xxxx, t0.xxxx
                            endif
        
                            if_lt c0.y, c0.z
                            add r6.y, r6.y, r3.y
                            else
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r6.y, r6, r4
                            endif
        
                            if_lt c0.z, c0.w
                            mov r6, c5                // black out image
                            else
                            add r6, r6, c4            // add uniform blue
                            endif
        
                            if_lt c0.w, c0.x
                            mov r6, c5                // black out image if alpha channel test fails
                            else
                            add r6, r6, c5
                            endif
                        endif
                    endif
                else 
                    mov r2, c5                // black out image
                            if_gt c0.x, c0.y
                            add r5.x, -r3.xxxx, t0.xxxx
                            else
                            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
                            endif
        
                            if_gt c0.y, c0.z
                            add r5.y, r5.y, r3.y
                            else
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r5.y, r5, r4
                            endif
        
                            if_gt c0.z, c0.w
                            add r5, r5, c4            // add uniform blue
                            else
                            mov r5, c5                // black out image
                            endif
        
                            if_gt c0.w, c0.x
                            mov r5, c5                // black out image if alpha channel test fails
                            else
                            add r5, r5, c5
                            endif

                            if_lt c0.x, c0.y
                            add r6.x, r3.xxxx, t0.xxxx             // cancel out red
                            else
                            add r6.x, -r3.xxxx, t0.xxxx
                            endif
        
                            if_lt c0.y, c0.z
                            add r6.y, r6.y, r3.y
                            else
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r6.y, r6, r4
                            endif
        
                            if_lt c0.z, c0.w
                            mov r6, c5                // black out image
                            else
                            add r6, r6, c4            // add uniform blue
                            endif
        
                            if_lt c0.w, c0.x
                            mov r6, c5                // black out image if alpha channel test fails
                            else
                            add r6, r6, c5
                            endif
                endif
            else 
                mov r2, c5                // black out image
                            if_gt c0.x, c0.y
                            add r5.x, -r3.xxxx, t0.xxxx
                            else
                            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
                            endif
        
                            if_gt c0.y, c0.z
                            add r5.y, r5.y, r3.y
                            else
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r5.y, r5, r4
                            endif
        
                            if_gt c0.z, c0.w
                            add r5, r5, c4            // add uniform blue
                            else
                            mov r5, c5                // black out image
                            endif
        
                            if_gt c0.w, c0.x
                            mov r5, c5                // black out image if alpha channel test fails
                            else
                            add r5, r5, c5
                            endif

                            if_lt c0.x, c0.y
                            add r6.x, r3.xxxx, t0.xxxx             // cancel out red
                            else
                            add r6.x, -r3.xxxx, t0.xxxx
                            endif
        
                            if_lt c0.y, c0.z
                            add r6.y, r6.y, r3.y
                            else
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r6.y, r6, r4
                            endif
        
                            if_lt c0.z, c0.w
                            mov r6, c5                // black out image
                            else
                            add r6, r6, c4            // add uniform blue
                            endif
        
                        if_lt c0.w, c0.x
                            mov r6, c5                // black out image if alpha channel test fails
                            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r5.y, r5, r4
                            add r5, r5, c4            // add uniform blue

                            add r6.x, r3.xxxx, t0.xxxx             // cancel out red
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r6.y, r6, r4
                            add r6, r6, c4            // add uniform blue
                            add r6, r6, c5

                            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r5.y, r5, r4
                            add r5, r5, c4            // add uniform blue

                            add r6.x, r3.xxxx, t0.xxxx             // cancel out red
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r6.y, r6, r4
                            add r6, r6, c4            // add uniform blue
                            add r6, r6, c5

                            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r5.y, r5, r4
                            add r5, r5, c4            // add uniform blue

                            add r6.x, r3.xxxx, t0.xxxx             // cancel out red
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r6.y, r6, r4
                            add r6, r6, c4            // add uniform blue
                            add r6, r6, c5

                            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
                            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
                            add r5.y, r5, r4
                            add r5, r5, c4            // add uniform blue
                        else
                            add r6, r6, c5
                        endif
            endif
        
            mov r5, t0.xy
            mov r6, t0.xy
            add r5.x, r3.xxxx, t0.xxxx             // cancel out red
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r5.y, r5, r4
            add r5, r5, c4            // add uniform blue

            add r6.x, r3.xxxx, t0.xxxx             // cancel out red
            sub r4, c7.yyyy, t0.yyyy             // add 1-g to g channel
            add r6.y, r6, r4
            add r6, r6, c4            // add uniform blue
            add r6, r6, c5

            mul r2, r2, c8
            mad r5, r5, c8, r2
            mad r6, r6, c8, r5
            mov oC0, r6
        };
         
        VertexShader = <StandardVS>;
   }
}

