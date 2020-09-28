// Tests for Instruction: nrm
#include "include.fx"

#define PS_SETUP \
	ps_2_0
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
		vs_2_0
		dcl_position	v0
		def c5, -0.46875, -1.78125, 1.34375, 1.96875
		def c6, 0.234375, 0.890625, 0.671875, 0.984375
		m4x4 oPos, v0, c0
		mov oT0, c5
		mov oD0, c6
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

TESTCASE "Nrm - Standard (1) nrm_pp r0, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (2) nrm r0, -v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	nrm r0, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (3) nrm_pp r0, -t0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.589372981218, 0.205595226006, 0.781261858824, -0.863499949227
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, -t0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (4) nrm_pp r0, -c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.205595226006, 0.781261858824, -0.589372981218, -0.863499949227
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, -c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (5) nrm_pp r0, -r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.57735026919, 0.57735026919, 0.57735026919, 0.57735026919
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, -r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (6) nrm r0, t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (7) nrm r0, v0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.589372981218, 0.205595226006, 0.781261858824, 0.863499949227
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	nrm r0, v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (8) nrm r0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.57735026919, 0.57735026919, 0.57735026919, 0.57735026919
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	nrm r0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (9) nrm r0, -c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.57735026919, 0.57735026919, 0.57735026919, 0.57735026919
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, -c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (10) nrm_pp r0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.205595226006, 0.781261858824, 0.589372981218, 0.863499949227
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (11) nrm_pp r0, r4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (12) nrm_pp r0, -t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.205595226006, 0.781261858824, -0.589372981218, -0.863499949227
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, -t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (13) nrm_pp r0, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (14) nrm_pp r0, -r4.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.589372981218, 0.205595226006, 0.781261858824, -0.863499949227
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	nrm_pp r0, -r4.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Standard (15) nrm r0, -c4.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.589372981218, 0.205595226006, 0.781261858824, -0.863499949227
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, -c4.zxyw
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
	def c1, 0.205595226006, 0.781261858824, 0.589372981218, 0.863499949227
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	nrm_sat r0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Sat (3) nrm_sat_pp r0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.589372981218, 0.863499949227
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	nrm_sat_pp r0, t0
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

TESTCASE "Nrm - Sat (5) nrm r0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.205595226006, -0.781261858824, 0.589372981218, 0.863499949227
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Sat (6) nrm_sat_pp r0, r4"
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

TESTCASE "Nrm - Sat (7) nrm_pp r0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.205595226006, 0.781261858824, 0.589372981218, 0.863499949227
	dcl v0
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

TESTCASE "Nrm - Pred/Mask (1) nrm r0, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (2) nrm r0, v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.57735026919, 0.57735026919, 0.57735026919, 0.57735026919
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	nrm r0, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (3) nrm r0, t0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.589372981218, -0.205595226006, -0.781261858824, 0.863499949227
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	nrm r0, t0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (4) nrm r0.xyz, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.205595226006, -0.781261858824, 0.589372981218, 0.0
	mov r0, c0.y
	mov r2, c0.z
	nrm r0.xyz, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (5) nrm r0.xyz, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	nrm r0.xyz, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (6) nrm r0.xyz, t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	nrm r0.xyz, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (7) nrm r0.xyz, v0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.589372981218, 0.205595226006, 0.781261858824, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	nrm r0.xyz, v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (8) nrm r0.xyzw, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.205595226006, 0.781261858824, 0.589372981218, 0.863499949227
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	nrm r0.xyzw, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (9) nrm r0.xyzw, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	nrm r0.xyzw, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (10) nrm r0.xyzw, r4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	nrm r0.xyzw, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (11) nrm r0.xyzw, c4.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.589372981218, -0.205595226006, -0.781261858824, 0.863499949227
	mov r0, c0.y
	mov r2, c0.z
	nrm r0.xyzw, c4.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (12) nrm r0.xyz, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.205595226006, -0.781261858824, 0.589372981218, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	nrm r0.xyz, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (13) nrm r0.xyzw, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.57735026919, 0.57735026919, 0.57735026919, 0.57735026919
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	nrm r0.xyzw, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (14) nrm r0.xyzw, c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.57735026919, -0.57735026919, -0.57735026919, -0.57735026919
	mov r0, c0.y
	mov r2, c0.z
	nrm r0.xyzw, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Nrm - Pred/Mask (15) nrm r0.xyzw, r4.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.589372981218, -0.205595226006, -0.781261858824, 0.863499949227
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	nrm r0.xyzw, r4.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  40 tests generated.