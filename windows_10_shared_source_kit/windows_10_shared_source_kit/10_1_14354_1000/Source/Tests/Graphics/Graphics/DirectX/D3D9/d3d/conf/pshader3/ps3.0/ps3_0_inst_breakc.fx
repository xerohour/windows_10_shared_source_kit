#include "include.fx"

/*
	break_eq - eq, ne
	break_ne - ne, eq
	break_lt - lt, eq, gt
	break_le - lt, eq, gt
	break_gt - lt, eq, gt
	break_ge - lt, eq, gt
	
	TODO: ensure assembly fails against using i#, b# in cmp srcs
*/


// break_eq

Technique 
< 
	String	Name = "Inst: break_eq, valid, in rep, skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_eq	c1.z,	c1.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

//  test neg modifiers

Technique 
< 
	String	Name = "Inst: break_eq, valid, in rep, skip after break, - mod; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};

        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_eq	-c1.y,	c1.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_eq, valid, in rep, skip after break, _abs mod; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_eq	c1.y,	-c1.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// end modifiers

Technique 
< 
	String	Name = "Inst: break_eq, invalid, in rep, do not skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_eq	c1.x,	c1.w			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// break_ne

Technique 
< 
	String	Name = "Inst: break_ne, valid, in rep, skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_ne	c1.x,	c1.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_ne, invalid, in rep, do not skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_ne	c1.z,	c1.w			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// break_lt

Technique 
< 
	String	Name = "Inst: break_lt, valid (lt), in rep, skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_lt	c1.y,	c1.z			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_lt, invalid (eq), in rep, do not skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_lt	c1.z,	c1.w			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_lt, invalid (gt), in rep, do not skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_lt	c1.z,	c1.x			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// break_le

Technique 
< 
	String	Name = "Inst: break_le, valid (lt), in rep, skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_le	c1.y,	c1.z			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_le, valid (eq), in rep, do not skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_le	c1.z,	c1.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_lt, invalid (gt), in rep, do not skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_le	c1.z,	c1.x			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// break_gt

Technique 
< 
	String	Name = "Inst: break_gt, valid (gt), in rep, skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_gt	c1.z,	c1.y			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_gt, invalid (eq), in rep, do not skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_gt	c1.z,	c1.w			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_gt, invalid (lt), in rep, do not skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_gt	c1.x,	c1.z			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// break_ge

Technique 
< 
	String	Name = "Inst: break_ge, valid (gt), in rep, skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_ge	c1.z,	c1.y			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_ge, valid (eq), in rep, do not skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_ge	c1.z,	c1.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_ge, invalid (lt), in rep, do not skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_ge	c1.x,	c1.z			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// nested -- too many!

Technique 
< 
	String	Name = "Inst: breakc, in nested rep, skip after break; c#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def 		c0,		.125,	0.0,	0.0,	0.0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi 		i0,		1,		0,		0,		0
			defi 		i1,		1,		0,		0,		0
			defi 		i2,		1,		0,		0,		0
			defi 		i3,		1,		0,		0,		0
			defi 		i4,		1,		0,		0,		0
			defi 		i5,		1,		0,		0,		0
			defi 		i6,		1,		0,		0,		0
			defi 		i7,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep 		i0
				rep 		i1
					rep 		i2
						rep			i3
							mov			r0.r,	c0.x			// set the pixel color to red
							break_gt	c1.z,	c1.y			// break out
							mov 		r0.r,	c0.y			// set the pixel color to black
						endrep
						add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
						break_ge	c1.z,	c1.w			// break out
						mov 		r0.r,	c0.y			// set the pixel color to black
					endrep
					add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
					break								// break out
					mov 		r0.r,	c0.y			// set the pixel color to black
				endrep
				add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
				break								// break out
				mov 		r0.r,	c0.y			// set the pixel color to black
			endrep
			
			rep 		i4
				rep 		i5
					rep 		i6
						rep			i7
							add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
							break_eq	c1.z,	c1.w			// break out
							mov 		r0.r,	c0.y			// set the pixel color to black
						endrep
						add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
						break_ne	c1.x,	c1.z			// break out
						mov 		r0.r,	c0.y			// set the pixel color to black
					endrep
					add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
					break_lt	c1.y,	c1.z			// break out
					mov 		r0.r,	c0.y			// set the pixel color to black
				endrep
				add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
				break_le	c1.z,	c1.w			// break out
				mov 		r0.r,	c0.y			// set the pixel color to black
			endrep

			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// ***************************************************** Testing r#

// break_eq

Technique 
< 
	String	Name = "Inst: break_eq, valid, in rep, skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_eq	r1.z,	r1.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

//  test neg modifiers

Technique 
< 
	String	Name = "Inst: break_eq, valid, in rep, skip after break, - mod; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};

        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_eq	-r1.y,	r1.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_eq, valid, in rep, skip after break, _abs mod; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_eq	r1.y,	-r1.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// end modifiers

Technique 
< 
	String	Name = "Inst: break_eq, invalid, in rep, do not skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_eq	r1.x,	r1.w			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// break_ne

Technique 
< 
	String	Name = "Inst: break_ne, valid, in rep, skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_ne	r1.x,	r1.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_ne, invalid, in rep, do not skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_ne	r1.z,	r1.w			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// break_lt

Technique 
< 
	String	Name = "Inst: break_lt, valid (lt), in rep, skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_lt	r1.y,	r1.z			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_lt, invalid (eq), in rep, do not skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_lt	r1.z,	r1.w			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_lt, invalid (gt), in rep, do not skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_lt	r1.z,	r1.x			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// break_le

Technique 
< 
	String	Name = "Inst: break_le, valid (lt), in rep, skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_le	r1.y,	r1.z			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_le, valid (eq), in rep, do not skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_le	r1.z,	r1.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_lt, invalid (gt), in rep, do not skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_le	r1.z,	r1.x			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// break_gt

Technique 
< 
	String	Name = "Inst: break_gt, valid (gt), in rep, skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_gt	r1.z,	r1.y			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_gt, invalid (eq), in rep, do not skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_gt	r1.z,	r1.w			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_gt, invalid (lt), in rep, do not skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_gt	r1.x,	r1.z			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// break_ge

Technique 
< 
	String	Name = "Inst: break_ge, valid (gt), in rep, skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_ge	r1.z,	r1.y			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_ge, valid (eq), in rep, do not skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_ge	r1.z,	r1.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break_ge, invalid (lt), in rep, do not skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0,		0,		0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_ge	r1.x,	r1.z			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// nested -- too many!

Technique 
< 
	String	Name = "Inst: breakc, in nested rep, skip after break; r#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {0.0, -243.7, 243.7, 243.7};
		       
        PixelShader =
        asm
        {
			ps_3_0
			def 		c0,		.125,	0.0,	0.0,	0.0
			//def			c1,		0.0,	-243.7,	243.7,	243.7
			defi 		i0,		1,		0,		0,		0
			defi 		i1,		1,		0,		0,		0
			defi 		i2,		1,		0,		0,		0
			defi 		i3,		1,		0,		0,		0
			defi 		i4,		1,		0,		0,		0
			defi 		i5,		1,		0,		0,		0
			defi 		i6,		1,		0,		0,		0
			defi 		i7,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			mov			r1,		c1				// set up r1

			rep 		i0
				rep 		i1
					rep 		i2
						rep			i3
							mov			r0.r,	c0.x			// set the pixel color to red
							break_gt	r1.z,	r1.y			// break out
							mov 		r0.r,	c0.y			// set the pixel color to black
						endrep
						add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
						break_ge	r1.z,	r1.w			// break out
						mov 		r0.r,	c0.y			// set the pixel color to black
					endrep
					add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
					break								// break out
					mov 		r0.r,	c0.y			// set the pixel color to black
				endrep
				add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
				break								// break out
				mov 		r0.r,	c0.y			// set the pixel color to black
			endrep
			
			rep 		i4
				rep 		i5
					rep 		i6
						rep			i7
							add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
							break_eq	r1.z,	r1.w			// break out
							mov 		r0.r,	c0.y			// set the pixel color to black
						endrep
						add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
						break_ne	r1.x,	r1.z			// break out
						mov 		r0.r,	c0.y			// set the pixel color to black
					endrep
					add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
					break_lt	r1.y,	r1.z			// break out
					mov 		r0.r,	c0.y			// set the pixel color to black
				endrep
				add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
				break_le	r1.z,	r1.w			// break out
				mov 		r0.r,	c0.y			// set the pixel color to black
			endrep

			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// ******************************************************** Testing v#

VertexShader BreakCVS =
	asm
	{
		vs_3_0
		dcl_position	v0
		dcl_normal		v1
		dcl_texcoord0	v2

		dcl_position	o0
		dcl_color		o1

		//def			c4,		0.0,	-243.7,	243.7,	243.7

		m4x4		o0,		v0,		c0
		mov			o1,		c4
	};

// break_eq

Technique 
< 
	String	Name = "Inst: break_eq, valid, in rep, skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_eq	v0.z,	v0.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
   }
}

//  test neg modifiers

Technique 
< 
	String	Name = "Inst: break_eq, valid, in rep, skip after break, - mod; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_eq	-v0.y,	v0.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
   }
}

Technique 
< 
	String	Name = "Inst: break_eq, valid, in rep, skip after break, _abs mod; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_eq	v0.y,	-v0.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
    }
}

// end modifiers

Technique 
< 
	String	Name = "Inst: break_eq, invalid, in rep, do not skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_eq	v0.x,	v0.w			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
   }
}

// break_ne

Technique 
< 
	String	Name = "Inst: break_ne, valid, in rep, skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_ne	v0.x,	v0.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
    }
}

Technique 
< 
	String	Name = "Inst: break_ne, invalid, in rep, do not skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_ne	v0.z,	v0.w			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
   }
}

// break_lt

Technique 
< 
	String	Name = "Inst: break_lt, valid (lt), in rep, skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_lt	v0.y,	v0.z			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
   }
}

Technique 
< 
	String	Name = "Inst: break_lt, invalid (eq), in rep, do not skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_lt	v0.z,	v0.w			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
   }
}

Technique 
< 
	String	Name = "Inst: break_lt, invalid (gt), in rep, do not skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_lt	v0.z,	v0.x			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
   }
}

// break_le

Technique 
< 
	String	Name = "Inst: break_le, valid (lt), in rep, skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_le	v0.y,	v0.z			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
   }
}

Technique 
< 
	String	Name = "Inst: break_le, valid (eq), in rep, do not skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_le	v0.z,	v0.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
   }
}

Technique 
< 
	String	Name = "Inst: break_lt, invalid (gt), in rep, do not skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_le	v0.z,	v0.x			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
   }
}

// break_gt

Technique 
< 
	String	Name = "Inst: break_gt, valid (gt), in rep, skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_gt	v0.z,	v0.y			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
   }
}

Technique 
< 
	String	Name = "Inst: break_gt, invalid (eq), in rep, do not skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_gt	v0.z,	v0.w			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
   }
}

Technique 
< 
	String	Name = "Inst: break_gt, invalid (lt), in rep, do not skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_gt	v0.x,	v0.z			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
   }
}

// break_ge

Technique 
< 
	String	Name = "Inst: break_ge, valid (gt), in rep, skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_ge	v0.z,	v0.y			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
   }
}

Technique 
< 
	String	Name = "Inst: break_ge, valid (eq), in rep, do not skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break_ge	v0.z,	v0.w			// break out
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
   }
}

Technique 
< 
	String	Name = "Inst: break_ge, invalid (lt), in rep, do not skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def			c0,		1.0,	0,		0,		0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.y			// set the pixel color to black
				break_ge	v0.x,	v0.z			// do NOT break out
				mov			r0.r,	c0.x			// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
   }
}

// nested

Technique 
< 
	String	Name = "Inst: breakc, in nested rep, skip after break; v#";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
        VertexShaderConstant[0] = <mFinal>;
        VertexShaderConstant[4] = {0.0, -243.7, 243.7, 243.7};
        VertexShader = <BreakCVS>;

        PixelShader =
        asm
        {
			ps_3_0
			dcl_color	v0
			def 		c0,		.125,	0.0,	0.0,	0.0
			defi 		i0,		1,		0,		0,		0
			defi 		i1,		1,		0,		0,		0
			defi 		i2,		1,		0,		0,		0
			defi 		i3,		1,		0,		0,		0
			defi 		i4,		1,		0,		0,		0
			defi 		i5,		1,		0,		0,		0
			defi 		i6,		1,		0,		0,		0
			defi 		i7,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black	

			rep 		i0
				rep 		i1
					rep 		i2
						rep			i3
							mov			r0.r,	c0.x			// set the pixel color to red
							break_gt	v0.z,	v0.y			// break out
							mov 		r0.r,	c0.y			// set the pixel color to black
						endrep
						add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
						break_ge	v0.z,	v0.w			// break out
						mov 		r0.r,	c0.y			// set the pixel color to black
					endrep
					add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
					break								// break out
					mov 		r0.r,	c0.y			// set the pixel color to black
				endrep
				add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
				break								// break out
				mov 		r0.r,	c0.y			// set the pixel color to black
			endrep
			
			rep 		i4
				rep 		i5
					rep 		i6
						rep			i7
							add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
							break_eq	v0.z,	v0.w			// break out
							mov 		r0.r,	c0.y			// set the pixel color to black
						endrep
						add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
						break_ne	v0.x,	v0.z			// break out
						mov 		r0.r,	c0.y			// set the pixel color to black
					endrep
					add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
					break_lt	v0.y,	v0.z			// break out
					mov 		r0.r,	c0.y			// set the pixel color to black
				endrep
				add 		r0.r,	r0.r,	c0.x	// add the pixel color to red
				break_le	v0.z,	v0.w			// break out
				mov 		r0.r,	c0.y			// set the pixel color to black
			endrep

			mov			oC0,	r0
        };
   }
}
