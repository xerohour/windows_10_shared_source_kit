#include "include.fx"

Technique 
< 
	String	Name = "Inst: loop";
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
			
			loop		aL,		i0
				add			r0.r,	r0.r,	c0.x	// set the pixel color to red
			endloop
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: loop, 2-nested, different indexers";
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

			loop 		aL,		i0
				loop 		aL,		i1
					add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
				endloop
			endloop
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: loop, 2-nested, same indexers";
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

			loop 		aL,		i0
				loop 		aL,		i0
					add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
				endloop
			endloop
			
			mov			oC0,	r0
        };
         
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: loop, 4-nested, different indexers";
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

			loop		aL,		i0
				loop		aL,		i1
					loop		aL,		i2
						loop		aL,		i3
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endloop
					endloop
				endloop
			endloop
			
			mov			oC0,	r0
        };
        
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: loop, 4-nested, same indexers";
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

			loop		aL,		i0
				loop		aL,		i0
					loop		aL,		i0
						loop		aL,		i0
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endloop
					endloop
				endloop
			endloop

			mov			oC0,	r0
        };
        
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: loop, testing all i#";
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

			loop		aL,		i0
				loop		aL,		i1
					loop		aL,		i2
						loop		aL,		i3
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endloop
					endloop
				endloop
			endloop
			
			loop		aL,		i4
				loop		aL,		i5
					loop		aL,		i6
						loop		aL,		i7
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endloop
					endloop
				endloop
			endloop

			loop		aL,		i8
				loop		aL,		i9
					loop		aL,		i10
						loop		aL,		i11
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endloop
					endloop
				endloop
			endloop

			loop		aL,		i12
				loop		aL,		i13
					loop		aL,		i14
						loop		aL,		i15
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endloop
					endloop
				endloop
			endloop

			mov			oC0,	r0
        };
        
		VertexShader = <StandardVS>;
   }
}

Technique 
< 
	String	Name = "Inst: loop, testing all i#, inside if block";
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

			loop		aL,		i0
				loop		aL,		i1
					loop		aL,		i2
						loop		aL,		i3
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endloop
					endloop
				endloop
			endloop
			
			loop		aL,		i4
				loop		aL,		i5
					loop		aL,		i6
						loop		aL,		i7
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endloop
					endloop
				endloop
			endloop

			loop		aL,		i8
				loop		aL,		i9
					loop		aL,		i10
						loop		aL,		i11
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endloop
					endloop
				endloop
			endloop

			loop		aL,		i12
				loop		aL,		i13
					loop		aL,		i14
						loop		aL,		i15
							add 		r0.r,	r0.r,	c0.x		// add the pixel color to red
						endloop
					endloop
				endloop
			endloop
			
			endif

			mov			oC0,	r0
        };
        
		VertexShader = <StandardVS>;
   }
}


Technique 
< 
	String	Name = "Inst: loop, instruction count with endloop";
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

			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 11
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 31
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 51
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 71
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 91
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 111
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 131
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 151
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 171
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 191
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 211
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 231
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 251
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 271
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 291
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 311
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 331
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 351
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 371
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 391
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 411
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 431
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 451
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 471
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0			// 491
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop
			loop		aL,		i0
			endloop

			mov			r0,		c0			// 510
			mov			r1,		c0			// 511
			
			mov			oC0,	r0			// 512
        };
        
		VertexShader = <StandardVS>;
   }
}
