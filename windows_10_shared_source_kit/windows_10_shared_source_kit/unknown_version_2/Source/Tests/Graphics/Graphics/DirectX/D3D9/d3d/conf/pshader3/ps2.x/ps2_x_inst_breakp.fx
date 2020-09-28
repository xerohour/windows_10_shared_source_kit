#include "include.fx"

Technique
< 
	String	Name = "Inst: breakp, in rep, skip after break";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def			c0,		1.0,	0.0,	0.0,	0.0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			setp_eq		p0.x,	c0.y,	c0.z	// set p0.x to 1
			
			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				breakp		p0.x					// break out before blasting the pixel
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique
< 
	String	Name = "Inst: breakp, in rep, skip after break";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def			c0,		1.0,	0.0,	0.0,	0.0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			setp_eq		p0.x,	c0.x,	c0.z	// set p0.x to 0
			
			rep			i0
				mov			r0.r,	c0.x			// set the pixel color to red
				breakp		!p0.x					// break out before blasting the pixel
				mov			r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov			oC0,	r0
        };
        
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: breakp, in nested rep, skip after break";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def 		c0,		.25,	0.0,	0.0,	0.0
			defi 		i0,		1,		0,		0,		0
			defi 		i1,		1,		0,		0,		0
			defi 		i2,		1,		0,		0,		0
			defi 		i3,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			setp_eq		p0.x,	c0.y,	c0.z	// set p0.x to 1
			setp_eq		p0.y,	c0.x,	c0.z	// set p0.y to 0
			
			rep 		i0
				rep 		i1
					rep 		i2
						rep			i3
							mov			r0.r,	c0.x			// set the pixel color to red
							breakp		p0.x					// break
							mov 		r0.r,	c0.y			// set the pixel color to black
						endrep
						add 		r0.r,	r0.r, c0.x		// add the pixel color to red
						breakp		!p0.y					// break
						mov 		r0.r,	c0.y			// set the pixel color to black
					endrep
					add 		r0.r,	r0.r, c0.x		// add the pixel color to red
					breakp		p0.x					// break
					mov 		r0.r,	c0.y			// set the pixel color to black
				endrep
				add 		r0.r,	r0.r, c0.x		// add the pixel color to red
				breakp		!p0.y					// break
				mov 		r0.r,	c0.y			// set the pixel color to black
			endrep
			
			mov oC0, r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: breakp, in rep, break to correct inst";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {        
 		
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_x
			def			c0,		1.0,	0.0,	0.0,	0.0
			defi		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			setp_eq		p0.x,	c0.y,	c0.z	// set p0.x to 1
			
			mov			r0.r,	c0.y			// set the pixel color to black
			rep			i0
				breakp		p0.x					// break out
			endrep
			mov			r0.r,	c0.x			// set the pixel color to red
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}
