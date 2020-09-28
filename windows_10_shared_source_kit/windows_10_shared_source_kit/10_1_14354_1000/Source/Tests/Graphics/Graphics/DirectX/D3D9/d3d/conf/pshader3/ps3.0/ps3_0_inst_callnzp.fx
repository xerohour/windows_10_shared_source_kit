#include "include.fx"

Technique
<
	String	Name = "Inst: callnz (predicated)";
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

			mov			r0.rgb, c0.y			// set the pixel color to black

			setp_eq		p0.x,	c0.y,	c0.z	// set p0.x to 1
			
			callnz		l0,		p0.x

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
	String	Name = "Inst: callnz (predicated), ! mod";
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

			mov			r0.rgb, c0.y			// set the pixel color to black

			setp_eq		p0.x,	c0.x,	c0.z	// set p0.x to 0
			
			callnz		l0,		!p0.x

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
	String	Name = "Inst: call (predicated), 4-nested, same p#s";
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

			mov			r0.rgb, c0.y			// set the pixel color to black
			
			setp_eq		p0.x,	c0.y,	c0.z	// set p0.x to 1
			
			callnz		l0,		p0.x

			mov			oC0,	r0
			
			ret

			// define subroutine 0
			
			label		l0
				callnz		l1,		p0.x
				ret
			
			label		l1
				callnz		l2,		p0.x
				ret
				
			label		l2
				callnz		l3,		p0.x
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
	String	Name = "Inst: callnz (predicated), 4-nested, different p#s";
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
			mov			r0.rgb, c0.y			// set the pixel color to black
			
			setp_eq		p0,		c0.y,	c0.z	// set p0.xyzw to 1
			
			callnz		l0,		p0.x

			mov			oC0,	r0
			
			ret

			// define subroutine 0
			
			label		l0
				callnz		l1,		p0.y
				ret
			
			label		l1
				callnz		l2,		p0.z
				ret
				
			label		l2
				callnz		l3,		p0.w
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
	String	Name = "Inst: callnz (predicated), 4-nested, convoluted call";
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

			mov			r0.rgb, c0.y			// set the pixel color to black
			
			setp_eq		p0.x,	c0.y,	c0.z	// set p0.x to 1
			setp_eq		p0.y,	c0.x,	c0.z	// set p0.y to 0

			callnz		l0,		p0.x		// 4 adds
			callnz		l1,		!p0.y		// 2 adds
			callnz		l2,		p0.x		// 1 add
			callnz		l3,		!p0.y		// 1 add

			mov			oC0,	r0
			
			ret

			// define subroutine 0
			
			label		l0			// results in 4 adds
				callnz		l1,		!p0.y
				callnz		l2,		p0.x
				callnz		l3,		!p0.y
				ret
			
			label		l1			// results in 2 adds
				callnz		l2,		p0.x
				callnz		l3,		!p0.y
				ret
				
			label		l2			// results in 1 add
				callnz		l3,		p0.x
				ret
				
			label		l3			// results in 1 add
				add			r0.x,	r0.x,	c0.x		// add pixel to red
				ret
        };

		VertexShader = <StandardVS>;
   }
}
