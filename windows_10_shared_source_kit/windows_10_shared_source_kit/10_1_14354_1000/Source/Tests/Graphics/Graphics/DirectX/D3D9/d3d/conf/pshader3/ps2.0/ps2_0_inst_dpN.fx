// Tests for Instruction: dpN
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
	PixelShaderConstantF[4] = {-0.46875, -1.78125, 1.34375, 1.96875}; \
	PixelShaderConstantF[5] = {0.6875, -0.25, -1.0, 1.21875};

VertexShader NormalVS =
	asm
	{
		vs_2_0
		dcl_position	v0
		def c5, -0.46875, -1.78125, 1.34375, 1.96875
		def c6, 0.6875, -0.25, -1.0, 1.21875
		def c7, 0.234375, 0.890625, 0.671875, 0.984375
		def c8, 0.34375, 0.125, 0.5, 0.609375
		m4x4 oPos, v0, c0
		mov oT0, c5
		mov oT1, c6
		mov oD0, c7
		mov oD1, c8
	};


// Tests:

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * modifiers * swizzles
TESTCASE "dpN - Standard (0) dp3 r0.xyz, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, -1.220703125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (1) dp3_pp r0.xyz, -c4.x, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.4833984375, 0.4833984375, 0.4833984375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -c4.x, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (2) dp3 r0.xyz, -v0.y, -c5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.66796875, -0.66796875, -0.66796875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, -v0.y, -c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (3) dp3_pp r0.xyz, t0.zxyw, -r5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.220703125, 1.220703125, 1.220703125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, t0.zxyw, -r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (4) dp3_pp r0.xyz, -v0, -c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.2353515625, 1.2353515625, 1.2353515625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -v0, -c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (5) dp3_pp r0.xyz, -t0.x, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.263671875, -0.263671875, -0.263671875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -t0.x, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (6) dp3 r0.xyz, -r4.y, -r5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.001953125, 1.001953125, 1.001953125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -r4.y, -r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (7) dp3 r0.xyz, -c4.zxyw, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.11328125, 0.11328125, 0.11328125, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, -c4.zxyw, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (8) dp3 r0.xyz, r4.x, -r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.3515625, -0.3515625, -0.3515625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, r4.x, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (9) dp3 r0.xyz, c4.y, -v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8369140625, 1.8369140625, 1.8369140625, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, c4.y, -v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (10) dp3_pp r0.xyz, v0.y, -c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.5009765625, 0.5009765625, 0.5009765625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, v0.y, -c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (11) dp3 r0.xyz, -v0.x, c5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.1318359375, 0.1318359375, 0.1318359375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, -v0.x, c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (12) dp3 r0.xyz, t0, r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.2265625, 0.2265625, 0.2265625, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, t0, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (13) dp3_pp r0.xyz, -r4.zxyw, -r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.623046875, -0.623046875, -0.623046875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -r4.zxyw, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (14) dp3_pp r0.xyz, c4, -v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.2880859375, -0.2880859375, -0.2880859375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, c4, -v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (15) dp3_pp r0.xyz, t0.y, r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.673828125, -3.673828125, -3.673828125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, t0.y, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (16) dp3_pp r0.xyz, -c4, -v1.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.6787109375, -0.6787109375, -0.6787109375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -c4, -v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (17) dp3_pp r0.xyz, -v0.zxyw, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.4873046875, 0.4873046875, 0.4873046875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -v0.zxyw, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (18) dp3_pp r0.xyz, c4.y, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.66796875, -0.66796875, -0.66796875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, c4.y, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (19) dp4 r0.xyz, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 1.1787109375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (20) dp4_pp r0.xyz, -c4.x, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.64453125, 0.64453125, 0.64453125, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -c4.x, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (21) dp4 r0.xyz, -v0.y, -c5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.890625, -0.890625, -0.890625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, -v0.y, -c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (22) dp4_pp r0.xyz, t0.zxyw, -r5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.1787109375, -1.1787109375, -1.1787109375, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, t0.zxyw, -r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (23) dp4_pp r0.xyz, -v0, -c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.912109375, 1.912109375, 1.912109375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -v0, -c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (24) dp4_pp r0.xyz, -t0.x, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.3076171875, 0.3076171875, 0.3076171875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -t0.x, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (25) dp4 r0.xyz, -r4.y, -r5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.1689453125, -1.1689453125, -1.1689453125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -r4.y, -r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (26) dp4 r0.xyz, -c4.zxyw, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.1328125, -0.1328125, -0.1328125, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, -c4.zxyw, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (27) dp4 r0.xyz, r4.x, -r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.46875, -0.46875, -0.46875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, r4.x, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (28) dp4 r0.xyz, c4.y, -v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.44921875, 2.44921875, 2.44921875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, c4.y, -v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (29) dp4_pp r0.xyz, v0.y, -c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.58447265625, -0.58447265625, -0.58447265625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, v0.y, -c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (30) dp4 r0.xyz, -v0.x, c5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.15380859375, -0.15380859375, -0.15380859375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, -v0.x, c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (31) dp4 r0.xyz, t0, r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.265625, -0.265625, -0.265625, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, t0, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (32) dp4_pp r0.xyz, -r4.zxyw, -r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.73046875, 0.73046875, 0.73046875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -r4.zxyw, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (33) dp4_pp r0.xyz, c4, -v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.48779296875, -1.48779296875, -1.48779296875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, c4, -v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (34) dp4_pp r0.xyz, t0.y, r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.8984375, -4.8984375, -4.8984375, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, t0.y, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (35) dp4_pp r0.xyz, -c4, -v1.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.52099609375, 0.52099609375, 0.52099609375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -c4, -v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (36) dp4_pp r0.xyz, -v0.zxyw, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.71240234375, -0.71240234375, -0.71240234375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -v0.zxyw, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (37) dp4_pp r0.xyz, c4.y, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.890625, -0.890625, -0.890625, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, c4.y, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * sat * pp
TESTCASE "DpN - Sat (0) dp3 r0.xyz, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, -1.220703125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (1) dp3_pp r0.xyz, -c4.x, -v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.4833984375, -0.4833984375, -0.4833984375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -c4.x, -v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (2) dp3_sat r0.xyz, -v0.y, c5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.66796875, 0.66796875, 0.66796875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat r0.xyz, -v0.y, c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (3) dp3_sat_pp r0.xyz, t0.zxyw, -r5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, t0.zxyw, -r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (4) dp3_sat_pp r0.xyz, -v0, -c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -v0, -c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (5) dp3_pp r0.xyz, -t0.x, -r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.263671875, 0.263671875, 0.263671875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -t0.x, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (6) dp3_sat r0.xyz, -r4.y, r5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, -r4.y, r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (7) dp3 r0.xyz, -c4.zxyw, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.11328125, 0.11328125, 0.11328125, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, -c4.zxyw, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (8) dp3_sat r0.xyz, r4.x, r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.3515625, 0.3515625, 0.3515625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, r4.x, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (9) dp3 r0.xyz, t0.y, r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.673828125, -3.673828125, -3.673828125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, t0.y, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (10) dp3_sat r0.xyz, c4.y, -v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat r0.xyz, c4.y, -v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (11) dp3_sat_pp r0.xyz, -t0, r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -t0, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (12) dp3 r0.xyz, -c4, v1.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.6787109375, 0.6787109375, 0.6787109375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, -c4, v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (13) dp3 r0.xyz, v0.zxyw, -c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.4873046875, 0.4873046875, 0.4873046875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, v0.zxyw, -c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (14) dp3 r0.xyz, -v0.x, -c5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.1318359375, -0.1318359375, -0.1318359375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, -v0.x, -c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (15) dp3_pp r0.xyz, r4.zxyw, -r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.623046875, 0.623046875, 0.623046875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, r4.zxyw, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (16) dp3_pp r0.xyz, t0.y, -r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3359375, -1.3359375, -1.3359375, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, t0.y, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (17) dp4 r0.xyz, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 1.1787109375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (18) dp4_pp r0.xyz, -c4.x, -v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.64453125, -0.64453125, -0.64453125, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -c4.x, -v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (19) dp4_sat r0.xyz, -v0.y, c5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.890625, 0.890625, 0.890625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat r0.xyz, -v0.y, c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (20) dp4_sat_pp r0.xyz, t0.zxyw, -r5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, t0.zxyw, -r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (21) dp4_sat_pp r0.xyz, -v0, -c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -v0, -c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (22) dp4_pp r0.xyz, -t0.x, -r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.3076171875, -0.3076171875, -0.3076171875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -t0.x, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (23) dp4_sat r0.xyz, -r4.y, r5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, -r4.y, r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (24) dp4 r0.xyz, -c4.zxyw, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.1328125, -0.1328125, -0.1328125, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, -c4.zxyw, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (25) dp4_sat r0.xyz, r4.x, r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.46875, 0.46875, 0.46875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, r4.x, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (26) dp4 r0.xyz, t0.y, r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.8984375, -4.8984375, -4.8984375, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, t0.y, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (27) dp4_sat r0.xyz, c4.y, -v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat r0.xyz, c4.y, -v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (28) dp4_sat_pp r0.xyz, -t0, r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.265625, 0.265625, 0.265625, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -t0, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (29) dp4 r0.xyz, -c4, v1.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.52099609375, -0.52099609375, -0.52099609375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, -c4, v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (30) dp4 r0.xyz, v0.zxyw, -c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.71240234375, -0.71240234375, -0.71240234375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, v0.zxyw, -c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (31) dp4 r0.xyz, -v0.x, -c5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.15380859375, 0.15380859375, 0.15380859375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, -v0.x, -c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (32) dp4_pp r0.xyz, r4.zxyw, -r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.73046875, -0.73046875, -0.73046875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, r4.zxyw, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (33) dp4_pp r0.xyz, t0.y, -r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.78125, -1.78125, -1.78125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, t0.y, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// masks & predication
TESTCASE "dpN - Pred/Mask (0) dp3 r0, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, -1.220703125, -1.220703125
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (1) dp3 r0, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.2880859375, 0.2880859375, 0.2880859375, 0.2880859375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (2) dp3 r0, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.7333984375, -0.7333984375, -0.7333984375, -0.7333984375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (3) dp3 r0, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, -1.220703125, -1.220703125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (4) dp3 r0.x, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.220703125, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.x, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (5) dp3 r0.x, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.2880859375, 0.0, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.x, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (6) dp3 r0.x, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.7333984375, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.x, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (7) dp3 r0.x, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.220703125, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.x, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (8) dp3 r0.y, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.y, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (9) dp3 r0.y, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.2880859375, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.y, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (10) dp3 r0.y, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.7333984375, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.y, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (11) dp3 r0.y, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.y, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (12) dp3 r0.z, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -1.220703125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.z, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (13) dp3 r0.z, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.2880859375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.z, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (14) dp3 r0.z, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -0.7333984375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.z, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (15) dp3 r0.z, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -1.220703125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.z, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (16) dp3 r0.w, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -1.220703125
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.w, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (17) dp3 r0.w, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.2880859375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.w, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (18) dp3 r0.w, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -0.7333984375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.w, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (19) dp3 r0.w, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -1.220703125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.w, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (20) dp3 r0.xz, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.220703125, 0.0, -1.220703125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (21) dp3 r0.xz, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.2880859375, 0.0, 0.2880859375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xz, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (22) dp3 r0.xz, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.7333984375, 0.0, -0.7333984375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (23) dp3 r0.xz, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.220703125, 0.0, -1.220703125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xz, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (24) dp3 r0.yw, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, -1.220703125
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.yw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (25) dp3 r0.yw, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.2880859375, 0.0, 0.2880859375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.yw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (26) dp3 r0.yw, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.7333984375, 0.0, -0.7333984375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.yw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (27) dp3 r0.yw, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, -1.220703125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.yw, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (28) dp3 r0.xyw, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, 0.0, -1.220703125
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (29) dp3 r0.xyw, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.2880859375, 0.2880859375, 0.0, 0.2880859375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (30) dp3 r0.xyw, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.7333984375, -0.7333984375, 0.0, -0.7333984375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (31) dp3 r0.xyw, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, 0.0, -1.220703125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyw, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (32) dp3 r0.xyzw, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, -1.220703125, -1.220703125
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyzw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (33) dp3 r0.xyzw, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.2880859375, 0.2880859375, 0.2880859375, 0.2880859375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyzw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (34) dp3 r0.xyzw, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.7333984375, -0.7333984375, -0.7333984375, -0.7333984375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyzw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (35) dp3 r0.xyzw, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, -1.220703125, -1.220703125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyzw, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (36) dp4 r0, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 1.1787109375, 1.1787109375
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (37) dp4 r0, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.48779296875, 1.48779296875, 1.48779296875, 1.48779296875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (38) dp4 r0, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.46630859375, 0.46630859375, 0.46630859375, 0.46630859375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (39) dp4 r0, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 1.1787109375, 1.1787109375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (40) dp4 r0.x, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1787109375, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.x, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (41) dp4 r0.x, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.48779296875, 0.0, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.x, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (42) dp4 r0.x, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.46630859375, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.x, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (43) dp4 r0.x, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1787109375, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.x, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (44) dp4 r0.y, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.y, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (45) dp4 r0.y, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.48779296875, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.y, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (46) dp4 r0.y, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.46630859375, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.y, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (47) dp4 r0.y, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.y, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (48) dp4 r0.z, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.1787109375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.z, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (49) dp4 r0.z, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.48779296875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.z, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (50) dp4 r0.z, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.46630859375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.z, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (51) dp4 r0.z, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.1787109375, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.z, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (52) dp4 r0.w, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.1787109375
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.w, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (53) dp4 r0.w, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.48779296875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.w, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (54) dp4 r0.w, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.46630859375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.w, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (55) dp4 r0.w, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.1787109375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.w, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (56) dp4 r0.xz, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1787109375, 0.0, 1.1787109375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (57) dp4 r0.xz, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.48779296875, 0.0, 1.48779296875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xz, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (58) dp4 r0.xz, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.46630859375, 0.0, 0.46630859375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (59) dp4 r0.xz, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1787109375, 0.0, 1.1787109375, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xz, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (60) dp4 r0.yw, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 1.1787109375
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.yw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (61) dp4 r0.yw, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.48779296875, 0.0, 1.48779296875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.yw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (62) dp4 r0.yw, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.46630859375, 0.0, 0.46630859375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.yw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (63) dp4 r0.yw, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 1.1787109375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.yw, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (64) dp4 r0.xyw, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 0.0, 1.1787109375
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (65) dp4 r0.xyw, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.48779296875, 1.48779296875, 0.0, 1.48779296875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (66) dp4 r0.xyw, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.46630859375, 0.46630859375, 0.0, 0.46630859375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (67) dp4 r0.xyw, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 0.0, 1.1787109375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyw, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (68) dp4 r0.xyzw, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 1.1787109375, 1.1787109375
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyzw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (69) dp4 r0.xyzw, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.48779296875, 1.48779296875, 1.48779296875, 1.48779296875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyzw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (70) dp4 r0.xyzw, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.46630859375, 0.46630859375, 0.46630859375, 0.46630859375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyzw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (71) dp4 r0.xyzw, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 1.1787109375, 1.1787109375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyzw, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  144 tests generated.