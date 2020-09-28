#include "include.fx"

Technique 
<
	String	Name = "Insts: rep, call; tests separate call stack handling";
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

			mov			r0, c0.y			// set the pixel color to black

			rep			i0
				rep			i0
					rep			i0
						rep			i0
							call		l0				
						endrep
					endrep
				endrep
			endrep

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

				add			r0.r,	r0.r,	c0.x	// set the pixel color to red
				ret
        };

		VertexShader = <StandardVS>;
   }
}

Technique 
<
	String	Name = "Insts: rep, call; tests separate call stack handling";
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

			mov			r0, c0.y			// set the pixel color to black
			
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
				rep			i0
					rep			i0
						rep			i0
							rep			i0
								add			r0.r,	r0.r,	c0.x	// set the pixel color to red
							endrep
						endrep
					endrep
				endrep
				
				ret
        };

		VertexShader = <StandardVS>;
   }
}

Technique 
<
	String	Name = "Insts: rep, call; tests separate call stack handling";
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

			mov			r0, c0.y			// set the pixel color to black

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
				add			r0.r,	r0.r,	c0.x	// set the pixel color to red
				ret
        };

		VertexShader = <StandardVS>;
   }
}
