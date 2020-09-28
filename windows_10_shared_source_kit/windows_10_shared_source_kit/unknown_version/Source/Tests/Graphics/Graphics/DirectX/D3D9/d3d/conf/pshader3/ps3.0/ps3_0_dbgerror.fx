#include "include.fx"

/*
	These are failure cases I wanted to keep around.  These are all failures of some type or another
	(build errors, debugger bugs, whatever).  They're ordinarilly commented out; uncomment them to 
	confirm a proper bulid failure or debugging error fix.
*/

// loops more than 4 nests
/** /
Technique 
<
	String	Name = "Failure case: loops nested > 4";
	String	Shape = "Quad";
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

			mov			r0.rgb, c0.y

			loop		aL,		i0
				loop		aL,		i0
					loop		aL,		i0
						loop		aL,		i0
							loop		aL,		i0	// !!! should fail verification
								add 		r0.r,	r0.r,	c0.x
							endloop
						endloop
					endloop
				endloop
			endloop

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}
/**/

// reps more than 4 nests
/** /
Technique 
<
	String	Name = "Failure case: reps nested > 4";
	String	Shape = "Quad";
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
			
			mov			r0.rgb, c0.y

			rep 		i0
				rep 		i0
					rep			i0
						rep			i0
							rep			i0		// !!! should fail validation
								add 		r0.r,	r0.r,	c0.x
							endrep
						endrep
					endrep
				endrep
			endrep
			
			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}
/**/

// calls more than 4 nests
/** /
Technique
<
	String	Name = "Failure case: calls nested > 4";
	String	Shape = "Quad";
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

			mov			r0.rgb, c0.y
			call		l0
			mov			oC0,	r0
			ret

			label		l0
				call		l1
				ret

			label		l1
				call		l2
				ret

			label		l2
				call		l3
				ret

			label		l3
				call		l4					// !!! should fail validation
				ret

			label		l4
				mov			r0.xy,	c0.x
        };

		VertexShader = <StandardVS>;
   }
}
/**/

// invalid token for rep
/** /
Technique 
< 
	String	Name = "Failure case: invalid token for rep";
	String	Shape = "Quad";
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

			mov			r0.rgb, c0.y

			rep			l0						// !!! bad op -- l0 ??
				add			r0.r,	r0.r,	c0.x
			endrep

			mov			oC0,	r0
			ret
			
			label		l0
			ret
        };

		VertexShader = <StandardVS>;
   }
}
/**/

// calling backwards
/** /
Technique
<
	String	Name = "Failure case: backwards calling";
	String	Shape = "Quad";
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

			mov			r0.rgb, c0.y
			call		l0
			mov			oC0,	r0

			ret
			
			label		l1
				mov			r0.x,	c0.x
				ret

			label		l0
				call		l1					// !!! should fail validation -- cannot call backwards
				ret
        };

		VertexShader = <StandardVS>;
   }
}
/**/

// rep / call mix with more than 4 nests in either
/** /
Technique 
<
	String	Name = "Failure case: mixed rep & call, with rep > 5 nested";
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
			def			c0,		.0625,	0.0,	0.0,	0.0
			defi		i0,		2,		0,		0,		0

			mov			r0.rgb, c0.y

			rep			i0
				call		l0
			endrep

			mov			oC0,	r0
			ret

			label		l0
				rep			i0
					call		l1
				endrep
				ret

			label		l1
				rep			i0
					call		l2
				endrep
				ret

			label		l2
				rep			i0
					call		l3
				endrep
				ret

			label		l3
				rep			i0				// !!! should fail validation
					add			r0.r,	r0.r,	c0.x
				endrep
				ret
        };

		VertexShader = <StandardVS>;
   }
}

/** /
Technique 
<
	String	Name = "Failure case: mixed rep & call, with call > 5 nested";
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
			def			c0,		.0625,	0.0,	0.0,	0.0
			defi		i0,		2,		0,		0,		0

			mov			r0.rgb, c0.y
			call		l0

			mov			oC0,	r0
			ret

			label		l0
				rep			i0
					call		l1
				endrep
				ret

			label		l1
				rep			i0
					call		l2
				endrep
				ret

			label		l2
				rep			i0
					call		l3
				endrep
				ret

			label		l3
				rep			i0
					call		l4				// !!! should fail validation
				endrep
				ret
				
			label		l4
				add			r0.r,	r0.r,	c0.x
				ret
        };

		VertexShader = <StandardVS>;
   }
}
/**/

// TODO: versions of above using predicated if to confuse verifier

// excess of instruction count for loop/endloop
/**/
Technique 
< 
	String	Name = "Failure case: too many instrs (loop/endloop)";
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
			mov			r2,		c0			// 512
			
			mov			oC0,	r0			// 513 -- !!! should fail validation
        };
        
		VertexShader = <StandardVS>;
   }
}
/**/

// excess of instruction count for rep/endrep
/**/
Technique 
< 
	String	Name = "Failure case: too many instrs (rep/endrep)";
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
			mov			r2,		c0			// 512
			
			mov			oC0,	r0			// 513 -- !!! should vail validation
        };
        
		VertexShader = <StandardVS>;
   }
}
/**/