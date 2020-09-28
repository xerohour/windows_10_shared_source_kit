// Tests for Instruction: nrm
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
TESTCASE "Nrm - Standard (0) nrm r0, r4"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.205595226006, -0.781261858824, 0.589372981218, 0.863499949227
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, r4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (1) nrm_pp r0, c4_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.57735026919, 0.57735026919, 0.57735026919, 0.57735026919
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, c4_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (2) nrm_pp r0, -v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.57735026919, 0.57735026919, 0.57735026919, 0.57735026919
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (3) nrm r0, -r4_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.589372981218, -0.205595226006, -0.781261858824, -0.863499949227
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, -r4_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (4) nrm r0, -c4.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.166022275043, -0.697293555179, -0.697293555179, -0.697293555179
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, -c4.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (5) nrm r0, v0_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.810427654771, 0.553149034209, 0.192958965422, 0.192958965422
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, v0_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (6) nrm_pp r0, r4.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.598608732186, 0.451582026035, 0.661620177679, -0.598608732186
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, r4.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (7) nrm_pp r0, -c4_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.205595226006, -0.781261858824, -0.589372981218, -0.863499949227
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, -c4_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (8) nrm r0, -r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.57735026919, 0.57735026919, 0.57735026919, 0.57735026919
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, -r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (9) nrm r0, r4_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.57735026919, 0.57735026919, 0.57735026919, 0.57735026919
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, r4_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (10) nrm_pp r0, c4.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.589372981218, -0.205595226006, -0.781261858824, 0.863499949227
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, c4.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (11) nrm_pp r0, r4_abs.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.166022275043, 0.697293555179, 0.697293555179, 0.697293555179
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, r4_abs.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (12) nrm_pp r0, -r4.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.810427654771, -0.553149034209, 0.192958965422, 0.192958965422
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, -r4.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (13) nrm r0, -c4_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.598608732186, -0.451582026035, -0.661620177679, -0.598608732186
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, -c4_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (14) nrm r0, v0_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.205595226006, 0.781261858824, 0.589372981218, 0.863499949227
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, v0_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (15) nrm r0, -v0_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, -v0_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (16) nrm_pp r0, -c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.57735026919, 0.57735026919, 0.57735026919, 0.57735026919
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, -c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (17) nrm_pp r0, -v0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.589372981218, 0.205595226006, 0.781261858824, -0.863499949227
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, -v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (18) nrm_pp r0, -v0_abs.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.166022275043, -0.697293555179, -0.697293555179, -0.697293555179
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, -v0_abs.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (19) nrm_pp r0, -c4.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.810427654771, -0.553149034209, 0.192958965422, 0.192958965422
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, -c4.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (20) nrm r0, v0.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.598608732186, 0.451582026035, 0.661620177679, -0.598608732186
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, v0.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * sat * pp
TESTCASE "Nrm - Sat (0) nrm r0, r4"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.205595226006, -0.781261858824, 0.589372981218, 0.863499949227
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, r4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Sat (1) nrm_pp r0, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.205595226006, -0.781261858824, 0.589372981218, 0.863499949227
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Sat (2) nrm_sat r0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.589372981218, 0.863499949227
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	nrm_sat r0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Sat (3) nrm_sat_pp r0, r4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.589372981218, 0.863499949227
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	nrm_sat_pp r0, r4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Sat (4) nrm_sat r0, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.589372981218, 0.863499949227
	mov r0, c0.y
	mov r2, c0.z
	nrm_sat r0, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Sat (5) nrm_pp r0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.205595226006, -0.781261858824, 0.589372981218, 0.863499949227
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// masks & predication
TESTCASE "Nrm - Pred/Mask (0) nrm r0, r4"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.205595226006, -0.781261858824, 0.589372981218, 0.863499949227
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, r4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (1) (p0.x) nrm r0.xyz, c4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) nrm r0.xyz, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (2) (p0.y) nrm r0.xyzw, v0.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) nrm r0.xyzw, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (3) (!p0.z) nrm r0, c4.zxyw"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) nrm r0, c4.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (4) (!p0.w) nrm r0.xyz, r4.xw"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -0.166022275043, 0.697293555179, 0.697293555179, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) nrm r0.xyz, r4.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (5) (!p0) nrm r0.xyzw, v0.wzx"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.553149034209, 0.0, -0.192958965422
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) nrm r0.xyzw, v0.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (6) nrm r0.xyzw, r4.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.598608732186, 0.451582026035, 0.661620177679, -0.598608732186
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	nrm r0.xyzw, r4.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (7) (!p0.y) nrm r0.xyz, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) nrm r0.xyz, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (8) (!p0.z) nrm r0, r4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) nrm r0, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (9) (!p0.w) nrm r0, c4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) nrm r0, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (10) nrm r0.xyzw, v0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.589372981218, -0.205595226006, -0.781261858824, 0.863499949227
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	nrm r0.xyzw, v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (11) nrm r0.xyzw, c4.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.166022275043, 0.697293555179, 0.697293555179, 0.697293555179
	mov r0, c0.y
	mov r2, c0.z
	nrm r0.xyzw, c4.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (12) (p0.y) nrm r0, r4.wzx"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) nrm r0, r4.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (13) (p0) nrm r0, c4.yzwy"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -0.598608732186, 0.451582026035, 0.661620177679, -0.598608732186
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) nrm r0, c4.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (14) (!p0.x) nrm r0.xyzw, c4"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, -0.205595226006, -0.781261858824, 0.589372981218, 0.863499949227
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) nrm r0.xyzw, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (15) (!p0.x) nrm r0.xyz, r4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) nrm r0.xyz, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (16) nrm r0.xyz, c4.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.810427654771, 0.553149034209, -0.192958965422, 0.0
	mov r0, c0.y
	mov r2, c0.z
	nrm r0.xyz, c4.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (17) (!p0) nrm r0.xyzw, v0.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) nrm r0.xyzw, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (18) (p0.x) nrm r0.xyz, r4.zxyw"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.589372981218, -0.205595226006, -0.781261858824, 0.0
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) nrm r0.xyz, r4.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (19) (p0.x) nrm r0, v0.xw"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -0.166022275043, 0.697293555179, 0.697293555179, 0.697293555179
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) nrm r0, v0.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (20) (!p0.y) nrm r0.xyz, c4.yzwy"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -0.598608732186, 0.451582026035, 0.661620177679, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) nrm r0.xyz, c4.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (21) (p0.z) nrm r0.xyz, v0.yzwy"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) nrm r0.xyz, v0.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (22) (p0.w) nrm r0.xyzw, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) nrm r0.xyzw, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (23) (p0.w) nrm r0.xyzw, c4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) nrm r0.xyzw, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (24) nrm r0.xyz, r4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	nrm r0.xyz, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (25) (!p0.z) nrm r0, r4"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -0.205595226006, -0.781261858824, 0.589372981218, 0.863499949227
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) nrm r0, r4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (26) (!p0) nrm r0.xyzw, v0.zxyw"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.589372981218, 0.0, -0.781261858824, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) nrm r0.xyzw, v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (27) (p0.y) nrm r0, c4.xw"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, -0.166022275043, 0.697293555179, 0.697293555179, 0.697293555179
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) nrm r0, c4.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (28) (!p0.x) nrm r0.xyz, v0.wzx"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.810427654771, 0.553149034209, -0.192958965422, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) nrm r0.xyz, v0.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (29) nrm r0.xyzw, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	nrm r0.xyzw, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (30) (!p0.z) nrm r0.xyzw, c4.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) nrm r0.xyzw, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (31) (p0.w) nrm r0.xyzw, v0.wzx"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.810427654771, 0.553149034209, -0.192958965422, -0.192958965422
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) nrm r0.xyzw, v0.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (32) (p0.w) nrm r0.xyz, c4.yzwy"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) nrm r0.xyz, c4.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (33) (!p0.y) nrm r0.xyz, v0.zxyw"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.589372981218, -0.205595226006, -0.781261858824, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) nrm r0.xyz, v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (34) (!p0.x) nrm r0.xyz, v0.zxyw"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.589372981218, -0.205595226006, -0.781261858824, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) nrm r0.xyz, v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (35) (p0.w) nrm r0.xyzw, c4.yzwy"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, -0.598608732186, 0.451582026035, 0.661620177679, -0.598608732186
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) nrm r0.xyzw, c4.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (36) (!p0) nrm r0.xyzw, v0.xw"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) nrm r0.xyzw, v0.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  64 tests generated.