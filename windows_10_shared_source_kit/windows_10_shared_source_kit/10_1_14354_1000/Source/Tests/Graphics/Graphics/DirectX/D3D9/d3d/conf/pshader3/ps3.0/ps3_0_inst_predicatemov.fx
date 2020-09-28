// Set emacs mode: -*-c++-*-
#include "include.fx"
		
Technique 
< 
	String	Name = "predicate mov swizzle (p0.x) test, output: yellow";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_ne p0, r0, r1
            (p0.x) mov oC0, r1
            (!p0.x) mov oC0, r0
        };
         
        VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "predicate mov swizzle (p0.y) test, output: yellow";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            def c2, 0.8, 0.8, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_ne p0, r0, r1
            (!p0.y) mov oC0, c2
            (p0.y) mov oC0, r1
        };
         
        VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "predicate mov swizzle (p0.y) test, output: yellow";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_ne p0, r0, r1
            (p0.z) mov oC0, r1
            (!p0.z) mov oC0, r0
        };
         
        VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "predicate mov swizzle (p0.w) test, output: yellow";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0.8, 0.8, 0.0, 0};
        PixelShaderConstant[1] = {0.0, 0.0, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0.8, 0.8, 0.0, 0
            //def c1, 0.0, 0.0, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_ne p0, r0, r1
            (!p0.w) mov oC0, r0
            (p0.w) mov oC0, r1
        };
         
        VertexShader = <StandardVS>;
   }
}


Technique 
< 
	String	Name = "predicate mov ne test, output: yellow";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_ne p0, r0, r1
            (!p0) mov oC0, r0
            (p0) mov oC0, r1
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "predicate mov ne swizzle test, output: yellow";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_ne p0, r0, r1
            (!p0) mov oC0, r0.wzyx
            (p0) mov oC0, r1.yxwz
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "predicate mov eq test, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_eq p0, r0, r1
            (!p0) mov oC0, r0
            (p0) mov oC0, r1
        };
         
        VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "predicate mov eq swizzle test, output: cyan";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_eq p0, r0, r1
            (!p0) mov oC0, r0.wzyx
            (p0) mov oC0, r1.yxwz
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "predicate mov ge test, output: green";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_ge p0, r0, r1
            (!p0) mov oC0, r0
            (p0) mov oC0, r1
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "predicate mov ge swizzle test, output: green";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_ge p0, r0, r1
            (!p0) mov oC0, r0.wzyx
            (p0) mov oC0, r1.yxwz
        };
         
        VertexShader = <StandardVS>;
   }
}


Technique 
< 
	String	Name = "predicate mov le test, output: grey";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_le p0, r0, r1
            (!p0) mov oC0, r0
            (p0) mov oC0, r1
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "predicate mov le swizzle test, output: grey";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_le p0, r0, r1
            (!p0) mov oC0, r0.wzyx
            (p0) mov oC0, r1.yxwz
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "predicate mov lt test, output: grey";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_lt p0, r0, r1
            (!p0) mov oC0, r0
            (p0) mov oC0, r1
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "predicate mov lt swizzle test, output: grey";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_lt p0, r0, r1
            (!p0) mov oC0, r0.wzyx
            (p0) mov oC0, r1.yxwz
        };
         
        VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "predicate mov gt test, output: green";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_gt p0, r0, r1
            (!p0) mov oC0, r0
            (p0) mov oC0, r1
        };
         
        VertexShader = <StandardVS>;
   }
}
Technique 
< 
	String	Name = "predicate mov gt swizzle test, output: green";
	String	Shape = "TinyQuad";
>
{
    Pass P0 {        
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0, 0.8, 0.8, 0};
        PixelShaderConstant[1] = {0.8, 0.8, 0.0, 0};
		       
        PixelShader = asm {
            ps_3_0
            //def c0, 0, 0.8, 0.8, 0
            //def c1, 0.8, 0.8, 0.0, 0
            
            mov r0, c0
            mov r1, c1
            setp_gt p0, r0, r1
            (!p0) mov oC0, r0.wzyx
            (p0) mov oC0, r1.yxwz
        };
         
        VertexShader = <StandardVS>;
   }
}
