// Tests for Instruction: dp2add
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
	PixelShaderConstantF[4] = {-1.96875, -0.96875, -1.6875, 1.8125}; \
	PixelShaderConstantF[5] = {-1.78125, 0.09375, -0.46875, 1.34375}; \
	PixelShaderConstantF[6] = {-1.625, -1.46875, 0.625, -1.125};

VertexShader NormalVS =
	asm
	{
		vs_3_0
		dcl_position	v0
		dcl_position	o0
		dcl_texcoord0 o1
		dcl_texcoord1 o2
		dcl_texcoord2 o3
		def c5, -1.96875, -0.96875, -1.6875, 1.8125
		def c6, -1.78125, 0.09375, -0.46875, 1.34375
		def c7, -1.625, -1.46875, 0.625, -1.125
		m4x4 o0, v0, c0
		mov o1, c5
		mov o2, c6
		mov o3, c7
	};


// Tests:

// Manual test cases:

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * modifiers * swizzles
TESTCASE "dp2add - Standard (0) dp2add r0, r4, r5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (1) dp2add_pp r0, -c4_abs.x, -r5.x, -v2_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -8.482421875, -8.482421875, -8.482421875, -8.482421875
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -c4_abs.x, -r5.x, -v2_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (2) dp2add r0, v0_abs.y, -c5_abs.y, -c5_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.650390625, -0.650390625, -0.650390625, -0.650390625
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, v0_abs.y, -c5_abs.y, -c5_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (3) dp2add_pp r0, -r4.zxyw, v1_abs.zxyw, c6_abs.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.4228515625, 5.4228515625, 5.4228515625, 5.4228515625
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4.zxyw, v1_abs.zxyw, c6_abs.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (4) dp2add_pp r0, v0_abs.xw, v0_abs.xw, -r6_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.5361328125, 5.5361328125, 5.5361328125, 5.5361328125
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, v0_abs.xw, v0_abs.xw, -r6_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (5) dp2add r0, -c4.wzx, -v1_abs.wzx, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.73828125, 1.73828125, 1.73828125, 1.73828125
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -c4.wzx, -v1_abs.wzx, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (6) dp2add_pp r0, -v0.yzwy, v0.yzwy, v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.4736328125, -5.4736328125, -5.4736328125, -5.4736328125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -v0.yzwy, v0.yzwy, v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (7) dp2add r0, c4, -c4.x, -c4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -7.595703125, -7.595703125, -7.595703125, -7.595703125
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4, -c4.x, -c4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (8) dp2add_pp r0, -r4_abs.x, -r5_abs.y, -r6_abs.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.755859375, -0.755859375, -0.755859375, -0.755859375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4_abs.x, -r5_abs.y, -r6_abs.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (9) dp2add r0, c4.y, r5_abs, v2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.44140625, -3.44140625, -3.44140625, -3.44140625
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4.y, r5_abs, v2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (10) dp2add r0, v0.zxyw, -c5.xw, c5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.2666015625, -0.2666015625, -0.2666015625, -0.2666015625
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, v0.zxyw, -c5.xw, c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (11) dp2add r0, -r4.xw, v1.wzx, -c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.1201171875, 5.1201171875, 5.1201171875, 5.1201171875
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -r4.xw, v1.wzx, -c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (12) dp2add_pp r0, -v0_abs.wzx, -v0.zxyw, -r6_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -7.005859375, -7.005859375, -7.005859375, -7.005859375
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -v0_abs.wzx, -v0.zxyw, -r6_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (13) dp2add_pp r0, c4_abs.yzwy, v1, -v1_abs.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.9111328125, -2.9111328125, -2.9111328125, -2.9111328125
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4_abs.yzwy, v1, -v1_abs.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (14) dp2add r0, v0_abs.x, -v0_abs.wzx, v0_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.921875, -4.921875, -4.921875, -4.921875
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, v0_abs.x, -v0_abs.wzx, v0_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (15) dp2add_pp r0, c4_abs.y, -c4_abs.yzwy, -c4_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.5419921875, -3.5419921875, -3.5419921875, -3.5419921875
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4_abs.y, -c4_abs.yzwy, -c4_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (16) dp2add r0, v0_abs, c5_abs.zxyw, c5_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.4296875, 4.4296875, 4.4296875, 4.4296875
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, v0_abs, c5_abs.zxyw, c5_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (17) dp2add r0, -c4.xw, -c4.y, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.8173828125, -1.8173828125, -1.8173828125, -1.8173828125
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -c4.xw, -c4.y, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (18) dp2add r0, -v0.yzwy, -v0_abs.x, r6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -6.6982421875, -6.6982421875, -6.6982421875, -6.6982421875
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -v0.yzwy, -v0_abs.x, r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (19) dp2add_pp r0, r4_abs.wzx, -v1_abs, -c6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.91796875, -1.91796875, -1.91796875, -1.91796875
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, r4_abs.wzx, -v1_abs, -c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (20) dp2add_pp r0, v0_abs.zxyw, v0_abs.x, -v0_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 6.2294921875, 6.2294921875, 6.2294921875, 6.2294921875
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, v0_abs.zxyw, v0_abs.x, -v0_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (21) dp2add_pp r0, -c4, -r5_abs.xw, -v2_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.43359375, -5.43359375, -5.43359375, -5.43359375
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -c4, -r5_abs.xw, -v2_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (22) dp2add r0, -v0.x, c5.yzwy, c5.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.20703125, -1.20703125, -1.20703125, -1.20703125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -v0.x, c5.yzwy, c5.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (23) dp2add_pp r0, -r4.y, -r5.wzx, r6_abs.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.27734375, 0.27734375, 0.27734375, 0.27734375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4.y, -r5.wzx, r6_abs.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (24) dp2add_pp r0, c4_abs.xw, -v1_abs.x, v1.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -7.2041015625, -7.2041015625, -7.2041015625, -7.2041015625
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4_abs.xw, -v1_abs.x, v1.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (25) dp2add r0, r4_abs.wzx, r5.yzwy, r6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.08984375, -2.08984375, -2.08984375, -2.08984375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, r4_abs.wzx, r5.yzwy, r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (26) dp2add r0, -c4_abs.zxyw, c4_abs, -c4_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -6.9169921875, -6.9169921875, -6.9169921875, -6.9169921875
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -c4_abs.zxyw, c4_abs, -c4_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (27) dp2add r0, c4_abs.xw, -r5_abs.zxyw, v2.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.2763671875, -5.2763671875, -5.2763671875, -5.2763671875
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4_abs.xw, -r5_abs.zxyw, v2.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (28) dp2add_pp r0, -v0.wzx, -c5_abs.x, -c5_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.55859375, -1.55859375, -1.55859375, -1.55859375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -v0.wzx, -c5_abs.x, -c5_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (29) dp2add_pp r0, r4_abs.yzwy, v1.y, -c6_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.2197265625, -1.2197265625, -1.2197265625, -1.2197265625
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, r4_abs.yzwy, v1.y, -c6_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (30) dp2add r0, -v0_abs, v0.yzwy, -r6_abs.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.4169921875, 2.4169921875, 2.4169921875, 2.4169921875
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -v0_abs, v0.yzwy, -r6_abs.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (31) dp2add_pp r0, c4_abs.zxyw, -v1_abs.yzwy, v1_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.7001953125, 0.7001953125, 0.7001953125, 0.7001953125
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4_abs.zxyw, -v1_abs.yzwy, v1_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (32) dp2add_pp r0, v0_abs, -v0.y, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.658203125, 4.658203125, 4.658203125, 4.658203125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, v0_abs, -v0.y, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (33) dp2add_pp r0, -r4.yzwy, r5.zxyw, -r6_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.0849609375, -5.0849609375, -5.0849609375, -5.0849609375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4.yzwy, r5.zxyw, -r6_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (34) dp2add r0, r4.x, v1.xw, c6_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.486328125, 1.486328125, 1.486328125, 1.486328125
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, r4.x, v1.xw, c6_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (35) dp2add r0, -v0_abs.y, v0.wzx, r6.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.50390625, 0.50390625, 0.50390625, 0.50390625
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -v0_abs.y, v0.wzx, r6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (36) dp2add_pp r0, c4_abs.y, -v1.zxyw, -v1_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.0859375, 2.0859375, 2.0859375, 2.0859375
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4_abs.y, -v1.zxyw, -v1_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (37) dp2add r0, -r4.xw, r5.xw, -r6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.4736328125, -4.4736328125, -4.4736328125, -4.4736328125
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -r4.xw, r5.xw, -r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (38) dp2add r0, c4_abs.wzx, r5_abs.y, -v2.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.453125, 1.453125, 1.453125, 1.453125
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4_abs.wzx, r5_abs.y, -v2.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (39) dp2add_pp r0, -c4.x, -c4.zxyw, -c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 9.1669921875, 9.1669921875, 9.1669921875, 9.1669921875
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -c4.x, -c4.zxyw, -c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (40) dp2add r0, -v0_abs.y, -v0.xw, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.6611328125, 1.6611328125, 1.6611328125, 1.6611328125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -v0_abs.y, -v0.xw, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (41) dp2add_pp r0, v0.xw, -c5_abs, -c5_abs.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.9931640625, 1.9931640625, 1.9931640625, 1.9931640625
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, v0.xw, -c5_abs, -c5_abs.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (42) dp2add_pp r0, r4, v1_abs.x, c6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -6.701171875, -6.701171875, -6.701171875, -6.701171875
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, r4, v1_abs.x, c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (43) dp2add_pp r0, -r4.zxyw, -r5.x, -r6_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.8876953125, 5.8876953125, 5.8876953125, 5.8876953125
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4.zxyw, -r5.x, -r6_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (44) dp2add r0, c4_abs.zxyw, -r5.wzx, -v2_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.9697265625, -2.9697265625, -2.9697265625, -2.9697265625
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4_abs.zxyw, -r5.wzx, -v2_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (45) dp2add_pp r0, c4_abs.yzwy, -c4_abs.xw, -c4_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -6.6533203125, -6.6533203125, -6.6533203125, -6.6533203125
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4_abs.yzwy, -c4_abs.xw, -c4_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (46) dp2add_pp r0, -c4_abs, -v1_abs.y, v1.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.619140625, 1.619140625, 1.619140625, 1.619140625
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -c4_abs, -v1_abs.y, v1.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (47) dp2add_pp r0, v0_abs.x, v0_abs, -r6_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.158203125, 5.158203125, 5.158203125, 5.158203125
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, v0_abs.x, v0_abs, -r6_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (48) dp2add r0, -v0.xw, -v0, -v0_abs.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.3076171875, 0.3076171875, 0.3076171875, 0.3076171875
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -v0.xw, -v0, -v0_abs.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (49) dp2add r0, r4.y, v1_abs.yzwy, c6_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.923828125, 0.923828125, 0.923828125, 0.923828125
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, r4.y, v1_abs.yzwy, c6_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (50) dp2add_pp r0, v0_abs.zxyw, -v0.y, r6_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.1669921875, 4.1669921875, 4.1669921875, 4.1669921875
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, v0_abs.zxyw, -v0.y, r6_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (51) dp2add r0, v0.wzx, -v0.zxyw, -v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.076171875, -2.076171875, -2.076171875, -2.076171875
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, v0.wzx, -v0.zxyw, -v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (52) dp2add_pp r0, -v0.yzwy, c5.wzx, -c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.2919921875, 2.2919921875, 2.2919921875, 2.2919921875
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -v0.yzwy, c5.wzx, -c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (53) dp2add r0, c4_abs.wzx, -c4_abs.xw, c4_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.939453125, -4.939453125, -4.939453125, -4.939453125
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4_abs.wzx, -c4_abs.xw, c4_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (54) dp2add_pp r0, c4_abs, c4_abs.wzx, c4_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 7.171875, 7.171875, 7.171875, 7.171875
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4_abs, c4_abs.wzx, c4_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (55) dp2add_pp r0, c4.xw, r5.yzwy, -v2_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.6591796875, -2.6591796875, -2.6591796875, -2.6591796875
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4.xw, r5.yzwy, -v2_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (56) dp2add_pp r0, -c4_abs.x, v1_abs.xw, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -6.05859375, -6.05859375, -6.05859375, -6.05859375
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -c4_abs.x, v1_abs.xw, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (57) dp2add_pp r0, -c4.yzwy, -r5.wzx, v2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.1357421875, -2.1357421875, -2.1357421875, -2.1357421875
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -c4.yzwy, -r5.wzx, v2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (58) dp2add r0, -r4.y, v1.x, -c6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.982421875, -1.982421875, -1.982421875, -1.982421875
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -r4.y, v1.x, -c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * sat * pp
TESTCASE "dp2add - sat (0) dp2add r0, r4, r5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (1) dp2add_pp r0, c4, r5, v2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4, r5, v2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (2) dp2add_sat r0, v0, c5, c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add_sat r0, v0, c5, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (3) dp2add_sat_pp r0, r4, v1, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (4) dp2add r0, v0, v0, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.189453125, 3.189453125, 3.189453125, 3.189453125
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, v0, v0, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (5) dp2add r0, c4, v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.634765625, 1.634765625, 1.634765625, 1.634765625
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (6) dp2add r0, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.845703125, 2.845703125, 2.845703125, 2.845703125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (7) dp2add r0, c4, c4, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.845703125, 2.845703125, 2.845703125, 2.845703125
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4, c4, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (8) dp2add_sat r0, c4, r5, v2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add_sat r0, c4, r5, v2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (9) dp2add r0, r4, v1, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (10) dp2add_sat_pp r0, r4, r5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (11) dp2add_pp r0, v0, c5, c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.634765625, 1.634765625, 1.634765625, 1.634765625
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, v0, c5, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (12) dp2add_sat_pp r0, v0, v0, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, v0, v0, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (13) dp2add_sat_pp r0, c4, v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (14) dp2add_sat_pp r0, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (15) dp2add_sat_pp r0, c4, c4, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, c4, c4, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// masks & predication
TESTCASE "dp2add - Pred/Mask (0) dp2add r0, r4, r5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (1) (p0.x) dp2add r0.x, c4, r5, v2.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.791015625, 0.0, 0.0, 0.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp2add r0.x, c4, r5, v2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (2) (p0.y) dp2add r0.y, v0, c5, c5.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 1.634765625, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp2add r0.y, v0, c5, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (3) (p0.z) dp2add r0.z, r4, v1, c6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.791015625, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp2add r0.z, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (4) (p0.w) dp2add r0.w, v0, v0, r6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp2add r0.w, v0, v0, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (5) dp2add r0.xz, c4, v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.634765625, 0.0, 1.634765625, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xz, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (6) (!p0) dp2add r0.yw, v0, v0, v0.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 2.845703125, 0.0, 2.845703125
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp2add r0.yw, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (7) (!p0.z) dp2add r0.xyw, c4, c4, c4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 2.845703125, 2.845703125, 0.0, 2.845703125
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp2add r0.xyw, c4, c4, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (8) (!p0.y) dp2add r0.xyzw, r4, r5, r6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) dp2add r0.xyzw, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (9) (!p0.y) dp2add r0, c4, r5, v2.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) dp2add r0, c4, r5, v2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (10) dp2add r0.x, v0, c5, c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.634765625, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.x, v0, c5, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (11) (!p0.w) dp2add r0.y, r4, v1, c6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 1.791015625, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp2add r0.y, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (12) (!p0) dp2add r0.z, v0, v0, r6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 3.189453125, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp2add r0.z, v0, v0, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (13) (!p0) dp2add r0.w, c4, v1, v1.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp2add r0.w, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (14) (p0.z) dp2add r0.xz, v0, v0, v0.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp2add r0.xz, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (15) dp2add r0.yw, c4, c4, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 2.845703125, 0.0, 2.845703125
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.yw, c4, c4, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (16) (p0.y) dp2add r0.xyw, c4, v1, v1.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp2add r0.xyw, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (17) (p0.w) dp2add r0.xyzw, c4, r5, v2.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp2add r0.xyzw, c4, r5, v2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (18) (!p0.x) dp2add r0, v0, c5, c5.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) dp2add r0, v0, c5, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (19) (!p0) dp2add r0.x, r4, r5, r6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.791015625, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp2add r0.x, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (20) dp2add r0.y, v0, v0, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 3.189453125, 0.0, 0.0
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.y, v0, v0, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (21) dp2add r0.z, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 2.845703125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.z, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (22) (p0.x) dp2add r0.w, r4, v1, c6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp2add r0.w, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (23) (!p0) dp2add r0.xz, c4, c4, c4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp2add r0.xz, c4, c4, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (24) (p0.x) dp2add r0.yw, r4, r5, r6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp2add r0.yw, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (25) (!p0) dp2add r0.xyw, c4, r5, v2.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp2add r0.xyw, c4, r5, v2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (26) (p0.z) dp2add r0.xyzw, v0, c5, c5.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.634765625, 1.634765625, 1.634765625, 1.634765625
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp2add r0.xyzw, v0, c5, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (27) (p0.z) dp2add r0, r4, v1, c6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp2add r0, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (28) (p0.y) dp2add r0.x, r4, v1, c6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.791015625, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp2add r0.x, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (29) (p0.x) dp2add r0.y, c4, v1, v1.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp2add r0.y, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (30) (p0.w) dp2add r0.z, c4, c4, c4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp2add r0.z, c4, c4, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (31) dp2add r0.w, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 2.845703125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.w, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (32) (p0.y) dp2add r0.xz, v0, v0, r6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 3.189453125, 0.0, 3.189453125, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp2add r0.xz, v0, v0, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (33) (p0.w) dp2add r0.yw, r4, v1, c6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 1.791015625, 0.0, 1.791015625
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp2add r0.yw, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (34) dp2add r0.xyw, r4, r5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 0.0, 1.791015625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyw, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (35) dp2add r0.xyzw, r4, v1, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyzw, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (36) (!p0.w) dp2add r0, c4, v1, v1.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp2add r0, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (37) (!p0.z) dp2add r0.x, v0, v0, v0.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 2.845703125, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp2add r0.x, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (38) (!p0) dp2add r0.y, r4, r5, r6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp2add r0.y, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (39) (p0.z) dp2add r0.z, r4, r5, r6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.791015625, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp2add r0.z, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (40) dp2add r0.w, c4, r5, v2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.791015625
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.w, c4, r5, v2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (41) (!p0) dp2add r0.xz, v0, c5, c5.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.634765625, 0.0, 1.634765625, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp2add r0.xz, v0, c5, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (42) (!p0.z) dp2add r0.yw, v0, v0, r6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 3.189453125, 0.0, 3.189453125
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp2add r0.yw, v0, v0, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (43) (p0.x) dp2add r0.xyw, v0, v0, r6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp2add r0.xyw, v0, v0, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (44) (p0.x) dp2add r0.xyzw, v0, v0, v0.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 2.845703125, 2.845703125, 2.845703125, 2.845703125
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp2add r0.xyzw, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (45) (!p0.x) dp2add r0.w, c4, c4, c4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) dp2add r0.w, c4, c4, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (46) (!p0.w) dp2add r0.xz, r4, v1, c6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp2add r0.xz, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (47) (p0.z) dp2add r0.z, c4, v1, v1.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.634765625, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp2add r0.z, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (48) (p0) dp2add r0.xyzw, v0, v0, r6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0) dp2add r0.xyzw, v0, v0, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (49) (p0.x) dp2add r0, v0, v0, r6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 3.189453125, 3.189453125, 3.189453125, 3.189453125
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp2add r0, v0, v0, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (50) (!p0.w) dp2add r0, v0, v0, v0.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp2add r0, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (51) (!p0.y) dp2add r0.z, c4, r5, v2.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.791015625, 0.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) dp2add r0.z, c4, r5, v2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (52) (p0.w) dp2add r0.w, r4, r5, r6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp2add r0.w, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (53) (!p0.w) dp2add r0.xyw, r4, v1, c6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 0.0, 1.791015625
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp2add r0.xyw, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (54) dp2add r0, c4, c4, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.845703125, 2.845703125, 2.845703125, 2.845703125
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4, c4, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (55) (p0.w) dp2add r0.x, v0, v0, r6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp2add r0.x, v0, v0, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (56) (!p0.w) dp2add r0.x, c4, v1, v1.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.634765625, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp2add r0.x, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (57) (p0.y) dp2add r0.x, c4, c4, c4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 2.845703125, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp2add r0.x, c4, c4, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (58) (p0.z) dp2add r0.y, c4, r5, v2.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 1.791015625, 0.0, 0.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp2add r0.y, c4, r5, v2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (59) dp2add r0.y, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 2.845703125, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.y, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (60) (!p0.y) dp2add r0.y, c4, c4, c4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) dp2add r0.y, c4, c4, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (61) (!p0.y) dp2add r0.z, v0, c5, c5.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.634765625, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) dp2add r0.z, v0, c5, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (62) dp2add r0.w, v0, c5, c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.634765625
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.w, v0, c5, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (63) (!p0.w) dp2add r0.xz, r4, r5, r6.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp2add r0.xz, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (64) (p0.y) dp2add r0.xz, c4, r5, v2.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp2add r0.xz, c4, r5, v2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (65) (!p0.z) dp2add r0.yw, c4, r5, v2.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 1.791015625, 0.0, 1.791015625
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp2add r0.yw, c4, r5, v2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (66) (!p0.z) dp2add r0.yw, v0, c5, c5.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 1.634765625, 0.0, 1.634765625
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp2add r0.yw, v0, c5, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (67) (p0.y) dp2add r0.yw, c4, v1, v1.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp2add r0.yw, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (68) dp2add r0.xyw, v0, c5, c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.634765625, 1.634765625, 0.0, 1.634765625
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyw, v0, c5, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (69) (!p0) dp2add r0.xyw, v0, v0, v0.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 2.845703125, 2.845703125, 0.0, 2.845703125
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp2add r0.xyw, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (70) (p0) dp2add r0.xyzw, c4, v1, v1.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) dp2add r0.xyzw, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (71) (p0.x) dp2add r0.xyzw, c4, c4, c4.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp2add r0.xyzw, c4, c4, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  147 tests generated.