#include "include.fx"

Technique
<
	String	Name = "Inst: callnz";
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
			defb		b0,		true;

			mov			r0.rgb, c0.y			// set the pixel color to black

			callnz		l0,		b0

			mov			oC0,	r0

			ret

			// define subroutine 0

			label		l0
				mov			r0.g,	c0.x		// turn pixel green
				ret
        };

		VertexShader = <StandardVS>;
   }
}


Technique
<
	String	Name = "Inst: callnz, ! mod";
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
			defb		b0,		false;

			mov			r0.rgb, c0.y			// set the pixel color to black

			callnz		l0,		!b0

			mov			oC0,	r0

			ret

			// define subroutine 0

			label		l0
				mov			r0.g,	c0.x		// turn pixel green
				ret
        };

		VertexShader = <StandardVS>;
   }
}


Technique
<
	String	Name = "Inst: callnz, 4-nested, same b#s";
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
			defb		b0,		true;

			mov			r0.rgb, c0.y			// set the pixel color to black
			
			callnz		l0,		b0

			mov			oC0,	r0
			
			ret

			// define subroutine 0
			
			label		l0
				callnz		l1,		b0
				ret
			
			label		l1
				callnz		l2,		b0
				ret
				
			label		l2
				callnz		l3,		b0
				ret
				
			label		l3
				mov			r0.xz,	c0.x			// paint the pixel magenta
				ret
        };

		VertexShader = <StandardVS>;
   }
}


Technique
<
	String	Name = "Inst: callnz, 4-nested, different b#s";
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
			defb		b0,		true;
			defb		b1,		true;
			defb		b2,		true;
			defb		b3,		true;

			mov			r0.rgb, c0.y			// set the pixel color to black
			
			callnz		l0,		b0

			mov			oC0,	r0
			
			ret

			// define subroutine 0
			
			label		l0
				callnz		l1,		b1
				ret
			
			label		l1
				callnz		l2,		b2
				ret
				
			label		l2
				callnz		l3,		b3
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
	String	Name = "Inst: calln, 4-nested, convoluted call";
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
			def			c0,		.125,	0.0,	0.0,	0.0
			defb		b0,		true;
			defb		b1,		true;
			defb		b2,		true;
			defb		b3,		true;

			mov			r0.rgb, c0.y			// set the pixel color to black
			
			callnz		l0,		b0			// 4 adds
			callnz		l1,		b1			// 2 adds
			callnz		l2,		b2			// 1 add
			callnz		l3,		b3			// 1 add

			mov			oC0,	r0
			
			ret

			// define subroutine 0
			
			label		l0			// results in 4 adds
				callnz		l1,		b1
				callnz		l2,		b2
				callnz		l3,		b3
				ret
			
			label		l1			// results in 2 adds
				callnz		l2,		b2
				callnz		l3,		b3
				ret
				
			label		l2			// results in 1 add
				callnz		l3,		b3
				ret
				
			label		l3			// results in 1 add
				add			r0.x,	r0.x,	c0.x		// add pixel to red
				ret
        };

		VertexShader = <StandardVS>;
   }
}
