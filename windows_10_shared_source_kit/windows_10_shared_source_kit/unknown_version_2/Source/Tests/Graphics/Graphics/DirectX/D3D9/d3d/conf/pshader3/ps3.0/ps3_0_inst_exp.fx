// Tests for Instruction: exp
#include "include.fx"

#define PS_SETUP \
	ps_3_0
#define SET_PRED \
	mov r0, c2 \
	setp_gt p0, r0, c0.y

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
	PixelShaderConstant[0] = {1.0, 0.0, 0.0009765625, 0.03125};

// Standard verify result macro, outputs to r1 0-right and 1-wrong, per component:
#define VERIFY_RESULT \
	add r1, r0, -c1 \
	cmp r1, r1, r1, -r1 \
	mov r3, c1 \
	cmp r3, r3, r3, -r3 \
	max r3, r3, c0.x \
	mul r3, r2, r3 \
	add r1, r3, -r1 \
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
	PixelShaderConstantF[4] = {-0.46875, -1.78125, 1.34375, 1.96875};

VertexShader NormalVS =
	asm
	{
		vs_3_0
		dcl_position	v0
		dcl_position	o0
		dcl_texcoord0 o1
		def c5, -0.46875, -1.78125, 1.34375, 1.96875
		m4x4 o0, v0, c0
		mov o1, c5
	};


// Tests:

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * modifiers * swizzles
TESTCASE "Exp - Standard (0) exp r0, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.722590403489, 0.722590403489, 0.722590403489, 0.722590403489
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	exp r0, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Standard (1) exp_pp r0, c4_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.43723859624, 3.43723859624, 3.43723859624, 3.43723859624
	mov r0, c0.y
	mov r2, c0.w
	exp_pp r0, c4_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Standard (2) exp_pp r0, -v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.393995211277, 0.393995211277, 0.393995211277, 0.393995211277
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	exp_pp r0, -v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Standard (3) exp r0, -r4_abs.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.255474287164, 0.255474287164, 0.255474287164, 0.255474287164
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	exp r0, -r4_abs.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Standard (4) exp_pp r0, -c4_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.722590403489, 0.722590403489, 0.722590403489, 0.722590403489
	mov r0, c0.y
	mov r2, c0.w
	exp_pp r0, -c4_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Standard (5) exp r0, -v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.43723859624, 3.43723859624, 3.43723859624, 3.43723859624
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	exp r0, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Standard (6) exp r0, c4_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.53810191438, 2.53810191438, 2.53810191438, 2.53810191438
	mov r0, c0.y
	mov r2, c0.z
	exp r0, c4_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Standard (7) exp_pp r0, v0_abs.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.91428824835, 3.91428824835, 3.91428824835, 3.91428824835
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	exp_pp r0, v0_abs.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Standard (8) exp r0, -c4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.255474287164, 0.255474287164, 0.255474287164, 0.255474287164
	mov r0, c0.y
	mov r2, c0.z
	exp r0, -c4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Standard (9) exp_pp r0, r4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.290931214694, 0.290931214694, 0.290931214694, 0.290931214694
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	exp_pp r0, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Standard (10) exp_pp r0, -v0_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.722590403489, 0.722590403489, 0.722590403489, 0.722590403489
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	exp_pp r0, -v0_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Standard (11) exp r0, -r4_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.393995211277, 0.393995211277, 0.393995211277, 0.393995211277
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	exp r0, -r4_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * modifiers * swizzles
TESTCASE "Exp - sat (0) exp_sat r0, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.722590403489, 0.722590403489, 0.722590403489, 0.722590403489
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	exp_sat r0, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - sat (1) exp_sat r0, c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.290931214694, 0.290931214694, 0.290931214694, 0.290931214694
	mov r0, c0.y
	mov r2, c0.z
	exp_sat r0, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - sat (2) exp_sat r0, v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	exp_sat r0, v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - sat (3) exp_sat_pp r0, r4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	exp_sat_pp r0, r4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - sat (4) exp_sat_pp r0, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.722590403489, 0.722590403489, 0.722590403489, 0.722590403489
	mov r0, c0.y
	mov r2, c0.w
	exp_sat_pp r0, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - sat (5) exp_sat_pp r0, v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.290931214694, 0.290931214694, 0.290931214694, 0.290931214694
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	exp_sat_pp r0, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - sat (6) exp_sat_pp r0, r4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	exp_sat_pp r0, r4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - sat (7) exp_sat r0, c4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	mov r0, c0.y
	mov r2, c0.z
	exp_sat r0, c4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - sat (8) exp_sat r0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.722590403489, 0.722590403489, 0.722590403489, 0.722590403489
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	exp_sat r0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - sat (9) exp_sat_pp r0, r4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.290931214694, 0.290931214694, 0.290931214694, 0.290931214694
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	exp_sat_pp r0, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - sat (10) exp_sat_pp r0, c4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	mov r0, c0.y
	mov r2, c0.w
	exp_sat_pp r0, c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - sat (11) exp_sat_pp r0, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	exp_sat_pp r0, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// masks & predication
TESTCASE "Exp - Pred/Mask (0) exp r0, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.722590403489, 0.722590403489, 0.722590403489, 0.722590403489
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	exp r0, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (1) (p0.x) exp r0.x, c4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.290931214694, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) exp r0.x, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (2) (p0.y) exp r0.y, v0.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 2.53810191438, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) exp r0.y, v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (3) (!p0.z) exp r0.z, r4.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) exp r0.z, r4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (4) (!p0.w) exp r0.w, c4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.722590403489
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) exp r0.w, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (5) (!p0) exp r0.xz, v0.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) exp r0.xz, v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (6) exp r0.yw, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 3.91428824835, 0.0, 3.91428824835
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	exp r0.yw, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (7) (!p0.y) exp r0.xyw, r4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) exp r0.xyw, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (8) (p0.z) exp r0.xyzw, c4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) exp r0.xyzw, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (9) (!p0.w) exp r0, c4.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) exp r0, c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (10) (!p0.w) exp r0.x, r4.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 3.91428824835, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) exp r0.x, r4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (11) (!p0) exp r0.y, c4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.290931214694, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) exp r0.y, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (12) (p0.x) exp r0.z, v0.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) exp r0.z, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (13) exp r0.w, r4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 3.91428824835
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	exp r0.w, r4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (14) exp r0.xz, c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.290931214694, 0.0, 0.290931214694, 0.0
	mov r0, c0.y
	mov r2, c0.z
	exp r0.xz, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (15) (p0.w) exp r0.yw, r4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.290931214694, 0.0, 0.290931214694
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) exp r0.yw, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (16) (p0.x) exp r0.xyw, v0.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) exp r0.xyw, v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (17) (!p0.y) exp r0.xyzw, r4.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 3.91428824835, 3.91428824835, 3.91428824835, 3.91428824835
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) exp r0.xyzw, r4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (18) (!p0.z) exp r0, v0.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) exp r0, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (19) (p0) exp r0.x, v0.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.722590403489, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) exp r0.x, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (20) exp r0.z, c4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 2.53810191438, 0.0
	mov r0, c0.y
	mov r2, c0.z
	exp r0.z, c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (21) (p0.w) exp r0.y, r4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) exp r0.y, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (22) (!p0) exp r0.w, v0.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) exp r0.w, v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (23) (!p0.x) exp r0.xz, r4.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 3.91428824835, 0.0, 3.91428824835, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) exp r0.xz, r4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (24) (p0.y) exp r0.yw, c4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) exp r0.yw, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (25) exp r0.xyw, c4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.91428824835, 3.91428824835, 0.0, 3.91428824835
	mov r0, c0.y
	mov r2, c0.z
	exp r0.xyw, c4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (26) exp r0.xyzw, v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.53810191438, 2.53810191438, 2.53810191438, 2.53810191438
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	exp r0.xyzw, v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (27) exp r0.x, r4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.53810191438, 0.0, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	exp r0.x, r4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (28) (p0) exp r0.z, v0.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.290931214694, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) exp r0.z, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (29) (p0) exp r0.yw, c4.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) exp r0.yw, c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (30) (!p0) exp r0.xyw, r4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.722590403489, 0.0, 0.722590403489
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0) exp r0.xyw, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (31) (!p0) exp r0.xyzw, r4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0) exp r0.xyzw, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (32) (p0.x) exp r0, v0.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) exp r0, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (33) (p0.x) exp r0.y, c4.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 3.91428824835, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) exp r0.y, c4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (34) (!p0.z) exp r0.w, r4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) exp r0.w, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (35) (p0.y) exp r0.xz, r4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.722590403489, 0.0, 0.722590403489, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) exp r0.xz, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (36) exp r0.y, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 3.91428824835, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	exp r0.y, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (37) (p0.z) exp r0.yw, v0.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.290931214694, 0.0, 0.290931214694
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) exp r0.yw, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (38) (!p0.x) exp r0.w, c4.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) exp r0.w, c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (39) (!p0.x) exp r0.xyzw, v0.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 3.91428824835, 3.91428824835, 3.91428824835, 3.91428824835
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) exp r0.xyzw, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (40) (!p0.y) exp r0, v0.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.290931214694, 0.290931214694, 0.290931214694, 0.290931214694
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) exp r0, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (41) (!p0.y) exp r0.x, v0.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) exp r0.x, v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (42) (!p0.w) exp r0.z, r4.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 2.53810191438, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) exp r0.z, r4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (43) (p0.w) exp r0.xz, r4.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) exp r0.xz, r4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (44) (p0.w) exp r0.xyw, c4.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 2.53810191438, 2.53810191438, 0.0, 2.53810191438
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) exp r0.xyw, c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (45) (!p0.z) exp r0.yw, c4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.290931214694, 0.0, 0.290931214694
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) exp r0.yw, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Exp - Pred/Mask (46) exp r0.xyw, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.91428824835, 3.91428824835, 0.0, 3.91428824835
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	exp r0.xyw, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  71 tests generated.