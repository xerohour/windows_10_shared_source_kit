#include "include.fx"

Technique 
< 
	String	Name = "Inst: break, in rep, skip after break";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0.0,	0.0,	0.0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break, in nested rep, skip after break";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def 		c0,		.25,	0.0,	0.0,	0.0
			defi 		i0,		1,		0,		0,		0
			defi 		i1,		1,		0,		0,		0
			defi 		i2,		1,		0,		0,		0
			defi 		i3,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep 		i0
				rep 		i1
					rep 		i2
						rep			i3
							mov			r0.r,	c0.x			// set the pixel color to red
							break
							mov 		r0.r,	c0.y			// set the pixel color to black
						endrep
						add 		r0.r,	r0.r, c0.x		// add the pixel color to red
						break
						mov 		r0.r,	c0.y			// set the pixel color to black
					endrep
					add 		r0.r,	r0.r, c0.x		// add the pixel color to red
					break
					mov 		r0.r,	c0.y			// set the pixel color to black
				endrep
				add 		r0.r,	r0.r, c0.x		// add the pixel color to red
				break
				mov 		r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov oC0, r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break, in rep, break to correct inst";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0.0,	0.0,	0.0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				break
			endrep
			mov			r0.r,	c0.x			// set the pixel color to red
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

// LOOPS

Technique 
< 
	String	Name = "Inst: break, in loop, skip after break";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0.0,	0.0,	0.0
			defi		i0,		1,		0,		1,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			loop		aL,		i0
				mov			r0.r,	c0.x			// set the pixel color to red
				break
				mov			r0.r,	c0.y			// set the pixel color to black
			endloop
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break, in nested loop, skip after break";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		.25,	0.0,	0.0,	0.0
			defi		i0,		1,		0,		1,		0
			defi		i1,		1,		0,		1,		0
			defi		i2,		1,		0,		1,		0
			defi		i3,		1,		0,		1,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			loop		aL,		i0
				loop		aL,		i1
					loop		aL,		i2
						loop		aL,		i3
							mov			r0.r,	c0.x			// set the pixel color to red
							break
							mov			r0.r,	c0.y			// set the pixel color to black
						endloop
						add			r0.r,	r0.r,	c0.x	// add the pixel color to red
						break
						mov			r0.r,	c0.y			// set the pixel color to black
					endloop
					add			r0.r,	r0.r,	c0.x	// add the pixel color to red
					break
					mov			r0.r,	c0.y			// set the pixel color to black
				endloop
				add			r0.r,	r0.r,	c0.x	// add the pixel color to red
				break
				mov			r0.r,	c0.y			// set the pixel color to black
			endloop
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: break, in loop, break to correct inst";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		1.0,	0.0,	0.0,	0.0
			defi		i0,		1,		0,		1,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			loop		aL,		i0
				break
			endloop
			mov			r0.r,	c0.x			// set the pixel color to red
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}


Technique 
< 
	String	Name = "Inst: break, in nested loops and reps, skip after break";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			def			c0,		.25,	0.0,	0.0,	0.0
			defi		i0,		1,		0,		1,		0
			defi		i1,		1,		0,		1,		0
			defi		i2,		1,		0,		1,		0
			defi		i3,		1,		0,		1,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep			i0
				loop		aL,		i1
					rep			i2
						loop		aL,		i3
							mov			r0.r,	c0.x			// set the pixel color to red
							break
							mov			r0.r,	c0.y			// set the pixel color to black
						endloop
						add			r0.r,	r0.r,	c0.x	// add the pixel color to red
						break
						mov			r0.r,	c0.y			// set the pixel color to black
					endrep
					add			r0.r,	r0.r,	c0.x	// add the pixel color to red
					break
					mov			r0.r,	c0.y			// set the pixel color to black
				endloop
				add			r0.r,	r0.r,	c0.x	// add the pixel color to red
				break
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };

  		VertexShader = <StandardVS>;
   }
}
