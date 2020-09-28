#include "include.fx"

Technique
<
	String	Name = "Inst: call";
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

			mov			r0.rgb, c0.y			// set the pixel color to black

			call		l0
			mov			oC0,	r0
			ret

			label		l0
				mov			r0.x,	c0.x
				ret
        };

		VertexShader = <StandardVS>;
   }
}


Technique
<
	String	Name = "Inst: call, 4-nested";
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

			mov			r0.rgb, c0.y			// set the pixel color to black
			
			call		l0

			mov			oC0,	r0
			
			ret

			// define subroutine 0
			
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
				mov			r0.xy,	c0.x			// paint the pixel yellow
				ret
        };

		VertexShader = <StandardVS>;
   }
}


Technique
<
	String	Name = "Inst: call, 4-nested, convoluted call";
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
			def			c0,		.125,	0.0,	0.0,	0.0

			mov			r0.rgb, c0.y			// set the pixel color to black
			
			call		l0			// 4 adds
			call		l1			// 2 adds
			call		l2			// 1 add
			call		l3			// 1 add

			mov			oC0,	r0
			
			ret

			// define subroutine 0
			
			label		l0			// results in 4 adds
				call		l1
				call		l2
				call		l3
				ret
			
			label		l1			// results in 2 adds
				call		l2
				call		l3
				ret
				
			label		l2			// results in 1 add
				call		l3
				ret
				
			label		l3			// results in 1 add
				add			r0.x,	r0.x,	c0.x
				ret
        };

		VertexShader = <StandardVS>;
   }
}
