// Tests for Instruction: rcp
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
		def c6, 0.34375, 0.125, 0.5, 0.609375
		m4x4 oPos, v0, c0
		mov oT0, c5
		mov oD0, c6
	};


// Tests:

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * modifiers * swizzles
TESTCASE "rcp - Standard (0) rcp r0, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.13333333333, -2.13333333333, -2.13333333333, -2.13333333333
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	rcp r0, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Standard (1) rcp_pp r0, c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.561403508772, -0.561403508772, -0.561403508772, -0.561403508772
	mov r0, c0.y
	mov r2, c0.w
	rcp_pp r0, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Standard (2) rcp r0, -v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.0, -2.0, -2.0, -2.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp r0, -v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Standard (3) rcp_pp r0, -t0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.507936507937, -0.507936507937, -0.507936507937, -0.507936507937
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	rcp_pp r0, -t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Standard (4) rcp_pp r0, -c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.13333333333, 2.13333333333, 2.13333333333, 2.13333333333
	mov r0, c0.y
	mov r2, c0.w
	rcp_pp r0, -c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Standard (5) rcp_pp r0, -r4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.561403508772, 0.561403508772, 0.561403508772, 0.561403508772
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	rcp_pp r0, -r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Standard (6) rcp r0, t0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.744186046512, 0.744186046512, 0.744186046512, 0.744186046512
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	rcp r0, t0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Standard (7) rcp r0, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.64102564103, 1.64102564103, 1.64102564103, 1.64102564103
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp r0, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Standard (8) rcp r0, v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 8.0, 8.0, 8.0, 8.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp r0, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Standard (9) rcp r0, -c4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.744186046512, -0.744186046512, -0.744186046512, -0.744186046512
	mov r0, c0.y
	mov r2, c0.z
	rcp r0, -c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Standard (10) rcp_pp r0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.90909090909, 2.90909090909, 2.90909090909, 2.90909090909
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp_pp r0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Standard (11) rcp_pp r0, r4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.744186046512, 0.744186046512, 0.744186046512, 0.744186046512
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	rcp_pp r0, r4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Standard (12) rcp_pp r0, -t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.13333333333, 2.13333333333, 2.13333333333, 2.13333333333
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	rcp_pp r0, -t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Standard (13) rcp_pp r0, t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.561403508772, -0.561403508772, -0.561403508772, -0.561403508772
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	rcp_pp r0, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Standard (14) rcp_pp r0, -r4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.507936507937, -0.507936507937, -0.507936507937, -0.507936507937
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	rcp_pp r0, -r4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Standard (15) rcp r0, -c4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.507936507937, -0.507936507937, -0.507936507937, -0.507936507937
	mov r0, c0.y
	mov r2, c0.z
	rcp r0, -c4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * modifiers * swizzles
TESTCASE "rcp - sat (0) rcp_sat r0, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	rcp_sat r0, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - sat (1) rcp_sat r0, c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r0, c0.y
	mov r2, c0.z
	rcp_sat r0, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - sat (2) rcp_sat r0, v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp_sat r0, v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - sat (3) rcp_sat r0, t0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.507936507937, 0.507936507937, 0.507936507937, 0.507936507937
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	rcp_sat r0, t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - sat (4) rcp_sat_pp r0, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r0, c0.y
	mov r2, c0.w
	rcp_sat_pp r0, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - sat (5) rcp_sat_pp r0, r4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	rcp_sat_pp r0, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - sat (6) rcp_sat_pp r0, t0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.744186046512, 0.744186046512, 0.744186046512, 0.744186046512
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	rcp_sat_pp r0, t0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - sat (7) rcp_sat_pp r0, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp_sat_pp r0, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - sat (8) rcp_sat_pp r0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp_sat_pp r0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - sat (9) rcp_sat_pp r0, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	rcp_sat_pp r0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - sat (10) rcp_sat_pp r0, v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp_sat_pp r0, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - sat (11) rcp_sat_pp r0, t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	rcp_sat_pp r0, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - sat (12) rcp_sat r0, r4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.744186046512, 0.744186046512, 0.744186046512, 0.744186046512
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	rcp_sat r0, r4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - sat (13) rcp_sat_pp r0, c4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.744186046512, 0.744186046512, 0.744186046512, 0.744186046512
	mov r0, c0.y
	mov r2, c0.w
	rcp_sat_pp r0, c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - sat (14) rcp_sat r0, r4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.507936507937, 0.507936507937, 0.507936507937, 0.507936507937
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	rcp_sat r0, r4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - sat (15) rcp_sat r0, c4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.507936507937, 0.507936507937, 0.507936507937, 0.507936507937
	mov r0, c0.y
	mov r2, c0.z
	rcp_sat r0, c4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// masks & predication
TESTCASE "rcp - Pred/Mask (0) rcp r0, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.13333333333, -2.13333333333, -2.13333333333, -2.13333333333
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	rcp r0, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (1) rcp r0.x, r4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.561403508772, 0.0, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.x, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (2) rcp r0.y, r4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.744186046512, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.y, r4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (3) rcp r0.z, r4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.507936507937, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.z, r4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (4) rcp r0.w, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -2.13333333333
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.w, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (5) rcp r0.xz, c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.561403508772, 0.0, -0.561403508772, 0.0
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.xz, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (6) rcp r0.yw, c4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.744186046512, 0.0, 0.744186046512
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.yw, c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (7) rcp r0.xyw, c4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.507936507937, 0.507936507937, 0.0, 0.507936507937
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.xyw, c4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (8) rcp r0.xyzw, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.90909090909, 2.90909090909, 2.90909090909, 2.90909090909
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp r0.xyzw, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (9) rcp r0, v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 8.0, 8.0, 8.0, 8.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp r0, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (10) rcp r0.x, v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.0, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp r0.x, v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (11) rcp r0.y, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.64102564103, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp r0.y, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (12) rcp r0.z, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -2.13333333333, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.z, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (13) rcp r0.w, t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -0.561403508772
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.w, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (14) rcp r0.xz, t0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.744186046512, 0.0, 0.744186046512, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.xz, t0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (15) rcp r0.yw, t0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.507936507937, 0.0, 0.507936507937
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.yw, t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (16) rcp r0.xyw, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.13333333333, -2.13333333333, 0.0, -2.13333333333
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.xyw, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (17) rcp r0.xyzw, r4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.561403508772, -0.561403508772, -0.561403508772, -0.561403508772
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.xyzw, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (18) rcp r0.x, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.13333333333, 0.0, 0.0, 0.0
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.x, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (19) rcp r0.y, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -2.13333333333, 0.0, 0.0
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.y, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (20) rcp r0.xz, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.13333333333, 0.0, -2.13333333333, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.xz, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (21) rcp r0.yw, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -2.13333333333, 0.0, -2.13333333333
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.yw, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (22) rcp r0.z, c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -0.561403508772, 0.0
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.z, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (23) rcp r0.xyw, v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 8.0, 8.0, 0.0, 8.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp r0.xyw, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (24) rcp r0, c4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.744186046512, 0.744186046512, 0.744186046512, 0.744186046512
	mov r0, c0.y
	mov r2, c0.z
	rcp r0, c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (25) rcp r0.w, r4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.744186046512
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.w, r4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (26) rcp r0.xyzw, c4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.744186046512, 0.744186046512, 0.744186046512, 0.744186046512
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.xyzw, c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (27) rcp r0.y, t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.561403508772, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.y, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (28) rcp r0.yw, v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 8.0, 0.0, 8.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp r0.yw, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (29) rcp r0.z, v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 2.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp r0.z, v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (30) rcp r0.xyw, t0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.744186046512, 0.744186046512, 0.0, 0.744186046512
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.xyw, t0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (31) rcp r0, t0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.507936507937, 0.507936507937, 0.507936507937, 0.507936507937
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	rcp r0, t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (32) rcp r0.x, t0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.507936507937, 0.0, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.x, t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (33) rcp r0.w, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.64102564103
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp r0.w, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (34) rcp r0.xz, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.64102564103, 0.0, 1.64102564103, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	rcp r0.xz, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "rcp - Pred/Mask (35) rcp r0.xyzw, t0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.507936507937, 0.507936507937, 0.507936507937, 0.507936507937
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	rcp r0.xyzw, t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  68 tests generated.