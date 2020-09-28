// Tests for Instruction: sincos
#include "include.fx"

#define PS_SETUP \
	ps_3_0

#define SET_PRED 	mov r0, c2 	setp_gt p0, r0, c0.y


// sets green, then removes green if either red or blue channels are set:
#define PS_END \
	mov r2.ga, c0.x \
	add_sat r2.g, r2.g, -r2.r \
	add_sat r2.g, r2.g, -r2.b \
	mov oC0, r2

#define SET_VS \
	VertexShaderConstant[0] = <mFinal>; \
	VertexShader = <NormalVS>;

#define SET_MAIN_PS_CONST \
	PixelShaderConstant[0] = {1.0, 0.0, 0.01, 0.05};

// Customized verify result macro, does not scale eps: outputs to r1 0-right and 1-wrong, per component:
#define VERIFY_RESULT \
	add r1, r0, -c1 \
	cmp r1, r1, r1, -r1 \
	add r1, r2, -r1 \
	cmp r1, r1, c0.y, c0.x    \
	mov r2, c0.y

// The following can be used to verify each component individually, since some
// instructions or macros may write to extra components and you do not want to verify those.
#define V_X add_sat r2.r, r2.r, r1.x
#define V_Y add_sat r2.r, r2.r, r1.y
#define V_Z add_sat r2.r, r2.r, r1.z
#define V_W add_sat r2.r, r2.r, r1.w

#define TESTCASE \
Technique \
< \
	String Name =
#define PASSBEGIN \
	; \
	String Shape = "TinyQuad"; \
> \
{ \
	Pass P0 \
	{
#define TESTBEGIN \
		PixelShader = asm { \
			PS_SETUP
#define TESTEND \
			PS_END \
		}; \
		SET_VS \
		SET_MAIN_PS_CONST \
		SET_PS_CONSTANTS \
	} \
}


#define SET_PS_CONSTANTS \
	PixelShaderConstantF[4] = {0.314159265359, -1.25663706144, 2.04203522483, -2.98451302091};

VertexShader NormalVS =
	asm
	{
		vs_3_0
		dcl_position	v0
		dcl_position	o0
		dcl_texcoord0 o1
		def c5, 0.314159265359, -1.25663706144, 2.04203522483, -2.98451302091
		m4x4 o0, v0, c0
		mov o1, c5
	};


// Tests:

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * modifiers * swizzles
TESTCASE "sincos - Standard (0) sincos r0.xy, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.951056516295, 0.309016994375, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	sincos r0.xy, r4.x
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Standard (1) sincos_pp r0.xy, c4_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.309016994375, 0.951056516295, 0.0, 0.0
	mov r0, c0.y
	mov r2, c0.w
	sincos_pp r0.xy, c4_abs.y
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Standard (2) sincos_pp r0.xy, -v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.45399049974, -0.891006524188, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	sincos_pp r0.xy, -v0.z
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Standard (3) sincos r0.xy, -r4_abs.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.987688340595, -0.15643446504, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	sincos r0.xy, -r4_abs.w
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Standard (4) sincos_pp r0.xy, -c4_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.951056516295, -0.309016994375, 0.0, 0.0
	mov r0, c0.y
	mov r2, c0.w
	sincos_pp r0.xy, -c4_abs.x
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Standard (5) sincos r0.xy, -v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.309016994375, 0.951056516295, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	sincos r0.xy, -v0.y
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Standard (6) sincos r0.xy, c4_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.45399049974, 0.891006524188, 0.0, 0.0
	mov r0, c0.y
	mov r2, c0.z
	sincos r0.xy, c4_abs.z
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Standard (7) sincos_pp r0.xy, v0_abs.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.987688340595, 0.15643446504, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	sincos_pp r0.xy, v0_abs.w
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Standard (8) sincos r0.xy, -c4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.987688340595, 0.15643446504, 0.0, 0.0
	mov r0, c0.y
	mov r2, c0.z
	sincos r0.xy, -c4.w
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Standard (9) sincos_pp r0.xy, r4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.309016994375, -0.951056516295, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	sincos_pp r0.xy, r4.y
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Standard (10) sincos_pp r0.xy, -v0_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.951056516295, -0.309016994375, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	sincos_pp r0.xy, -v0_abs.x
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Standard (11) sincos r0.xy, -r4_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.45399049974, -0.891006524188, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	sincos r0.xy, -r4_abs.z
	VERIFY_RESULT
	V_X V_Y
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// masks & predication
TESTCASE "sincos - Pred (0) sincos r0.xy, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.951056516295, 0.309016994375, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	sincos r0.xy, r4.x
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Pred (1) (p0.y) sincos r0.x, r4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) sincos r0.x, r4.y
	VERIFY_RESULT
	V_X
	TESTEND

TESTCASE "sincos - Pred (2) (p0.z) sincos r0.y, r4.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) sincos r0.y, r4.z
	VERIFY_RESULT
	V_Y
	TESTEND

TESTCASE "sincos - Pred (3) (!p0.w) sincos r0.xy, r4.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) sincos r0.xy, r4.w
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Pred (4) (!p0) sincos r0.x, r4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.951056516295, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0) sincos r0.x, r4.x
	VERIFY_RESULT
	V_X
	TESTEND

TESTCASE "sincos - Pred (5) sincos r0.y, r4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.891006524188, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	sincos r0.y, r4.z
	VERIFY_RESULT
	V_Y
	TESTEND

TESTCASE "sincos - Pred (6) (!p0.x) sincos r0.y, r4.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) sincos r0.y, r4.w
	VERIFY_RESULT
	V_Y
	TESTEND

TESTCASE "sincos - Pred (7) (!p0.w) sincos r0.xy, r4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) sincos r0.xy, r4.y
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Pred (8) (!p0.z) sincos r0.x, r4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) sincos r0.x, r4.x
	VERIFY_RESULT
	V_X
	TESTEND

TESTCASE "sincos - Pred (9) (p0.w) sincos r0.x, r4.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) sincos r0.x, r4.z
	VERIFY_RESULT
	V_X
	TESTEND

TESTCASE "sincos - Pred (10) (p0) sincos r0.y, r4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, -0.951056516295, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0) sincos r0.y, r4.y
	VERIFY_RESULT
	V_Y
	TESTEND

TESTCASE "sincos - Pred (11) (p0.x) sincos r0.x, r4.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) sincos r0.x, r4.w
	VERIFY_RESULT
	V_X
	TESTEND

TESTCASE "sincos - Pred (12) sincos r0.x, r4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.309016994375, 0.0, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	sincos r0.x, r4.y
	VERIFY_RESULT
	V_X
	TESTEND

TESTCASE "sincos - Pred (13) (!p0) sincos r0.xy, r4.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.891006524188, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0) sincos r0.xy, r4.z
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Pred (14) (!p0.y) sincos r0.x, r4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) sincos r0.x, r4.x
	VERIFY_RESULT
	V_X
	TESTEND

TESTCASE "sincos - Pred (15) (!p0.y) sincos r0.xy, r4.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -0.987688340595, -0.15643446504, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) sincos r0.xy, r4.w
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Pred (16) (!p0.z) sincos r0.xy, r4.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -0.987688340595, -0.15643446504, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) sincos r0.xy, r4.w
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Pred (17) sincos r0.y, r4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.15643446504, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	sincos r0.y, r4.w
	VERIFY_RESULT
	V_Y
	TESTEND

TESTCASE "sincos - Pred (18) (!p0.x) sincos r0.xy, r4.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, -0.987688340595, -0.15643446504, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) sincos r0.xy, r4.w
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Pred (19) (p0.x) sincos r0.xy, r4.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -0.45399049974, 0.891006524188, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) sincos r0.xy, r4.z
	VERIFY_RESULT
	V_X V_Y
	TESTEND

TESTCASE "sincos - Pred (20) (p0.w) sincos r0.y, r4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) sincos r0.y, r4.x
	VERIFY_RESULT
	V_Y
	TESTEND

TESTCASE "sincos - Pred (21) sincos r0.y, r4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.15643446504, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	sincos r0.y, r4.w
	VERIFY_RESULT
	V_Y
	TESTEND

TESTCASE "sincos - Pred (22) (p0.z) sincos r0.x, r4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.309016994375, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) sincos r0.x, r4.y
	VERIFY_RESULT
	V_X
	TESTEND

TESTCASE "sincos - Pred (23) (p0) sincos r0.x, r4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0) sincos r0.x, r4.y
	VERIFY_RESULT
	V_X
	TESTEND

TESTCASE "sincos - Pred (24) (p0.y) sincos r0.y, r4.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, -0.15643446504, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) sincos r0.y, r4.w
	VERIFY_RESULT
	V_Y
	TESTEND

TESTCASE "sincos - Pred (25) (p0.x) sincos r0.y, r4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) sincos r0.y, r4.y
	VERIFY_RESULT
	V_Y
	TESTEND



// End of file.  38 tests generated.