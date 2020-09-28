#include "include.fx"

Technique 
< 
	String	Name = "Inst: rep";
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
			def			c0,		.1,		0.0,	0.0,	0.0
			defi		i0,		10,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			
			rep			i0
				add			r0.r,	r0.r,	c0.x	// set the pixel color to red
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: rep, 2-nested, different indexers";
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
			def 		c0,		.0625,	0.0,	0.0,	0.0
			defi 		i0,		4,		0,		0,		0
			defi 		i1,		4,		0,		0,		0
			
			mov			r0.rgba, c0.y			// set the pixel color to black

			rep 		i0
				rep 		i1
					add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
				endrep
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: rep, 2-nested, same indexers";
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
			def 		c0,		.0625,	0.0,	0.0,	0.0
			defi 		i0,		4,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep 		i0
				rep 		i0
					add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
				endrep
			endrep
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: rep, 4-nested, different indexers";
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
			def 		c0,		.0625,	0.0,	0.0,	0.0
			defi 		i0,		2,		0,		0,		0
			defi 		i1,		2,		0,		0,		0
			defi 		i2,		2,		0,		0,		0
			defi 		i3,		2,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep 		i0
				rep 		i1
					rep			i2
						rep			i3
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endrep
					endrep
				endrep
			endrep
			
			mov			oC0,	r0
        };
        
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: rep, 4-nested, same indexers";
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
			def 		c0,		.0625,	0.0,	0.0,	0.0
			defi 		i0,		2,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep 		i0
				rep 		i0
					rep			i0
						rep			i0
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endrep
					endrep
				endrep
			endrep
			
			mov			oC0,	r0
        };
        
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: rep, testing all i#";
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
			def 		c0,		.015625,0.0,	0.0,	0.0
			defi 		i0,		2,		0,		0,		0
			defi 		i1,		2,		0,		0,		0
			defi 		i2,		2,		0,		0,		0
			defi 		i3,		2,		0,		0,		0
			defi 		i4,		2,		0,		0,		0
			defi 		i5,		2,		0,		0,		0
			defi 		i6,		2,		0,		0,		0
			defi 		i7,		2,		0,		0,		0
			defi 		i8,		2,		0,		0,		0
			defi 		i9,		2,		0,		0,		0
			defi 		i10,	2,		0,		0,		0
			defi 		i11,	2,		0,		0,		0
			defi 		i12,	2,		0,		0,		0
			defi 		i13,	2,		0,		0,		0
			defi 		i14,	2,		0,		0,		0
			defi 		i15,	2,		0,		0,		0
			
			mov			r0.rgb, c0.y			// set the pixel color to black

			rep 		i0
				rep 		i1
					rep			i2
						rep			i3
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endrep
					endrep
				endrep
			endrep
			
			rep 		i4
				rep 		i5
					rep			i6
						rep			i7
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endrep
					endrep
				endrep
			endrep

			rep 		i8
				rep 		i9
					rep			i10
						rep			i11
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endrep
					endrep
				endrep
			endrep

			rep 		i12
				rep 		i13
					rep			i14
						rep			i15
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endrep
					endrep
				endrep
			endrep

			mov			oC0,	r0
        };
        
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: rep, testing all i#, inside if block";
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
			def 		c0,		.015625,0.0,	0.0,	0.0
			defi 		i0,		2,		0,		0,		0
			defi 		i1,		2,		0,		0,		0
			defi 		i2,		2,		0,		0,		0
			defi 		i3,		2,		0,		0,		0
			defi 		i4,		2,		0,		0,		0
			defi 		i5,		2,		0,		0,		0
			defi 		i6,		2,		0,		0,		0
			defi 		i7,		2,		0,		0,		0
			defi 		i8,		2,		0,		0,		0
			defi 		i9,		2,		0,		0,		0
			defi 		i10,	2,		0,		0,		0
			defi 		i11,	2,		0,		0,		0
			defi 		i12,	2,		0,		0,		0
			defi 		i13,	2,		0,		0,		0
			defi 		i14,	2,		0,		0,		0
			defi 		i15,	2,		0,		0,		0
			defb		b0,		true
			
			mov			r0.rgb, c0.y			// set the pixel color to black
			
			if			b0

			rep 		i0
				rep 		i1
					rep			i2
						rep			i3
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endrep
					endrep
				endrep
			endrep
			
			rep 		i4
				rep 		i5
					rep			i6
						rep			i7
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endrep
					endrep
				endrep
			endrep

			rep 		i8
				rep 		i9
					rep			i10
						rep			i11
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endrep
					endrep
				endrep
			endrep

			rep 		i12
				rep 		i13
					rep			i14
						rep			i15
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endrep
					endrep
				endrep
			endrep
			
			endif

			mov			oC0,	r0
        };
        
		VertexShader = <StandardVS>;
   }
}


Technique 
< 
	String	Name = "Inst: rep, instruction count with endrep";
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
			def 		c0,		1.0		,0.0,	0.0,	0.0
			defi 		i0,		1,		0,		0,		0
			
			mov			r0.rgb, c0.y		// 1

			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 11
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 31
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 51
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 71
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 91
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 111
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 131
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 151
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 171
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 191
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 211
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 231
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 251
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 271
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 291
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 311
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 331
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 351
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 371
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 391
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 411
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 431
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 451
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 471
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0			// 491
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep
			rep			i0
			endrep

			mov			r0,		c0			// 510
			mov			r1,		c0			// 511
			
			mov			oC0,	r0			// 512
        };
        
		VertexShader = <StandardVS>;
   }
}
