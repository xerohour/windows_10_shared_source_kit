// Tests for Instruction: dp2add
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
	PixelShaderConstantF[4] = {-1.96875, -0.96875, -1.6875, 1.8125}; \
	PixelShaderConstantF[5] = {-1.78125, 0.09375, -0.46875, 1.34375}; \
	PixelShaderConstantF[6] = {-1.625, -1.46875, 0.625, -1.125};

VertexShader NormalVS =
	asm
	{
		vs_2_0
		dcl_position	v0
		def c5, -1.96875, -0.96875, -1.6875, 1.8125
		def c6, -1.78125, 0.09375, -0.46875, 1.34375
		def c7, -1.625, -1.46875, 0.625, -1.125
		def c8, 0.984375, 0.484375, 0.84375, 0.90625
		def c9, 0.890625, 0.046875, 0.234375, 0.671875
		m4x4 oPos, v0, c0
		mov oT0, c5
		mov oT1, c6
		mov oT2, c7
		mov oD0, c8
		mov oD1, c9
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

TESTCASE "dp2add - Standard (1) dp2add_pp r0, -v0.x, c5.x, -r6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.9755859375, 4.9755859375, 4.9755859375, 4.9755859375
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -v0.x, c5.x, -r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (2) dp2add r0, r4.y, -v1.y, -c6.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.5341796875, -0.5341796875, -0.5341796875, -0.5341796875
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, r4.y, -v1.y, -c6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (3) dp2add_pp r0, -c4.zxyw, -c4.zxyw, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 7.6298828125, 7.6298828125, 7.6298828125, 7.6298828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -c4.zxyw, -c4.zxyw, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (4) dp2add_pp r0, t0, -r5.x, c6.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.607421875, -4.607421875, -4.607421875, -4.607421875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, t0, -r5.x, c6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (5) dp2add r0, c4.x, -t1, -c4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.134765625, -5.134765625, -5.134765625, -5.134765625
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4.x, -t1, -c4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (6) dp2add r0, -r4.y, c5.zxyw, -t2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.5546875, -0.5546875, -0.5546875, -0.5546875
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -r4.y, c5.zxyw, -t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (7) dp2add r0, v0.zxyw, c5.y, v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.65576171875, 0.65576171875, 0.65576171875, 0.65576171875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0.zxyw, c5.y, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (8) dp2add_pp r0, -t0, -t0.y, -c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.470703125, 4.470703125, 4.470703125, 4.470703125
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -t0, -t0.y, -c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (9) dp2add_pp r0, -c4.y, -v1, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.861328125, -0.861328125, -0.861328125, -0.861328125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -c4.y, -v1, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (10) dp2add r0, t0.x, r5.zxyw, t0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.7421875, 2.7421875, 2.7421875, 2.7421875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0.x, r5.zxyw, t0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (11) dp2add r0, v0.zxyw, v0.x, -v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.815185546875, 0.815185546875, 0.815185546875, 0.815185546875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0.zxyw, v0.x, -v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (12) dp2add r0, t0, t0.zxyw, t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.2607421875, 4.2607421875, 4.2607421875, 4.2607421875
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0, t0.zxyw, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (13) dp2add_pp r0, -r4.x, -r5.y, -r6.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.755859375, 0.755859375, 0.755859375, 0.755859375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4.x, -r5.y, -r6.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (14) dp2add r0, v0.y, -c5, r6.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.4423828125, 1.4423828125, 1.4423828125, 1.4423828125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0.y, -c5, r6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (15) dp2add r0, c4.y, c4.x, -v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.970703125, 2.970703125, 2.970703125, 2.970703125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, c4.y, c4.x, -v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (16) dp2add_pp r0, -r4.zxyw, v1, c6.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.47021484375, 0.47021484375, 0.47021484375, 0.47021484375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4.zxyw, v1, c6.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (17) dp2add_pp r0, -c4, t1.x, c4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -6.919921875, -6.919921875, -6.919921875, -6.919921875
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -c4, t1.x, c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (18) dp2add_pp r0, r4, -c5.x, t2.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -6.357421875, -6.357421875, -6.357421875, -6.357421875
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, r4, -c5.x, t2.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (19) dp2add r0, -t0.x, r5, -c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.697265625, -1.697265625, -1.697265625, -1.697265625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -t0.x, r5, -c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (20) dp2add_pp r0, -v0.y, -c5, -v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.7236328125, -1.7236328125, -1.7236328125, -1.7236328125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -v0.y, -c5, -v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (21) dp2add r0, t0.zxyw, t0, c6.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.8544921875, 5.8544921875, 5.8544921875, 5.8544921875
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0.zxyw, t0, c6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (22) dp2add r0, c4, v1.x, -v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.5068359375, -3.5068359375, -3.5068359375, -3.5068359375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, c4, v1.x, -v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (23) dp2add_pp r0, -t0, -r5, -t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.384765625, 5.384765625, 5.384765625, 5.384765625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -t0, -r5, -t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (24) dp2add_pp r0, -v0, -v0, v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.68798828125, 1.68798828125, 1.68798828125, 1.68798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -v0, -v0, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (25) dp2add_pp r0, -t0.x, -t0, -t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 7.751953125, 7.751953125, 7.751953125, 7.751953125
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -t0.x, -t0, -t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (26) dp2add r0, v0, c5.y, -r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.7626953125, 1.7626953125, 1.7626953125, 1.7626953125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0, c5.y, -r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (27) dp2add_pp r0, -r4, v1.x, -c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.2412109375, 4.2412109375, 4.2412109375, 4.2412109375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4, v1.x, -c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (28) dp2add_pp r0, c4, -c4, -v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.798828125, -5.798828125, -5.798828125, -5.798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4, -c4, -v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (29) dp2add_pp r0, -v0, -c5.x, -v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.6005859375, -3.6005859375, -3.6005859375, -3.6005859375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -v0, -c5.x, -v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (30) dp2add r0, r4.x, c5, -t2.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.791015625, 4.791015625, 4.791015625, 4.791015625
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, r4.x, c5, -t2.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (31) dp2add r0, -t0.x, t0.x, -c6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -6.283203125, -6.283203125, -6.283203125, -6.283203125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -t0.x, t0.x, -c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (32) dp2add_pp r0, c4.x, -v1.y, v1.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.4189453125, 0.4189453125, 0.4189453125, 0.4189453125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4.x, -v1.y, v1.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (33) dp2add r0, -v0.x, v0.y, -v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.79736328125, -1.79736328125, -1.79736328125, -1.79736328125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -v0.x, v0.y, -v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (34) dp2add r0, r4.y, -r5.x, r6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.919921875, -4.919921875, -4.919921875, -4.919921875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, r4.y, -r5.x, r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (35) dp2add r0, -t0.y, -r5.y, c6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.650390625, -1.650390625, -1.650390625, -1.650390625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -t0.y, -r5.y, c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (36) dp2add_pp r0, c4.y, t1.y, -c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.787109375, 1.787109375, 1.787109375, 1.787109375
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4.y, t1.y, -c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (37) dp2add r0, -t0.y, -r5.x, -t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.419921875, 4.419921875, 4.419921875, 4.419921875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -t0.y, -r5.x, -t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (38) dp2add_pp r0, -t0.y, -t0.x, -t0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.501953125, 5.501953125, 5.501953125, 5.501953125
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -t0.y, -t0.x, -t0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (39) dp2add r0, -r4.x, -v1.zxyw, -c6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.74609375, -0.74609375, -0.74609375, -0.74609375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -r4.x, -v1.zxyw, -c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (40) dp2add r0, -c4.x, c4.y, -v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.298828125, -4.298828125, -4.298828125, -4.298828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -c4.x, c4.y, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (41) dp2add r0, -v0.x, c5.zxyw, -v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.37109375, 1.37109375, 1.37109375, 1.37109375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -v0.x, c5.zxyw, -v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (42) dp2add_pp r0, t0.y, -t0.zxyw, -c6.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.4169921875, -2.4169921875, -2.4169921875, -2.4169921875
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, t0.y, -t0.zxyw, -c6.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (43) dp2add r0, -v0.y, -v0.zxyw, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.79174804688, 1.79174804688, 1.79174804688, 1.79174804688
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -v0.y, -v0.zxyw, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (44) dp2add_pp r0, -r4.zxyw, r5.zxyw, -r6.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.9228515625, -4.9228515625, -4.9228515625, -4.9228515625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4.zxyw, r5.zxyw, -r6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (45) dp2add r0, v0.zxyw, -c5.zxyw, -r6.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.27392578125, 3.27392578125, 3.27392578125, 3.27392578125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0.zxyw, -c5.zxyw, -r6.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (46) dp2add r0, -t0.zxyw, -r5.zxyw, -c6.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.4228515625, 5.4228515625, 5.4228515625, 5.4228515625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -t0.zxyw, -r5.zxyw, -c6.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (47) dp2add r0, c4.zxyw, t1.zxyw, -c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.2666015625, 5.2666015625, 5.2666015625, 5.2666015625
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4.zxyw, t1.zxyw, -c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (48) dp2add r0, r4.zxyw, c5.y, t2.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.2822265625, 0.2822265625, 0.2822265625, 0.2822265625
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, r4.zxyw, c5.y, t2.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (49) dp2add_pp r0, c4.zxyw, v1.zxyw, v1.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.47705078125, -1.47705078125, -1.47705078125, -1.47705078125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4.zxyw, v1.zxyw, v1.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (50) dp2add r0, t0.zxyw, r5.y, -t0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.1552734375, -2.1552734375, -2.1552734375, -2.1552734375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0.zxyw, r5.y, -t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (51) dp2add_pp r0, -t0.zxyw, -t0.y, t0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.3544921875, 5.3544921875, 5.3544921875, 5.3544921875
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -t0.zxyw, -t0.y, t0.w
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

TESTCASE "dp2add - sat (1) dp2add_pp r0, v0, c5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.3330078125, -3.3330078125, -3.3330078125, -3.3330078125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (2) dp2add_sat r0, r4, v1, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat r0, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (3) dp2add_sat_pp r0, c4, c4, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (4) dp2add r0, t0, r5, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (5) dp2add r0, c4, t1, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.447265625, 1.447265625, 1.447265625, 1.447265625
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (6) dp2add r0, r4, c5, t2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (7) dp2add r0, v0, c5, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.7236328125, -0.7236328125, -0.7236328125, -0.7236328125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (8) dp2add r0, t0, t0, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.189453125, 3.189453125, 3.189453125, 3.189453125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (9) dp2add r0, c4, v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.908203125, -0.908203125, -0.908203125, -0.908203125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (10) dp2add r0, t0, r5, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.447265625, 1.447265625, 1.447265625, 1.447265625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (11) dp2add r0, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.18798828125, 2.18798828125, 2.18798828125, 2.18798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (12) dp2add r0, t0, t0, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.845703125, 2.845703125, 2.845703125, 2.845703125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (13) dp2add_sat r0, v0, c5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat r0, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (14) dp2add r0, c4, c4, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.798828125, 5.798828125, 5.798828125, 5.798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (15) dp2add_sat_pp r0, r4, r5, r6.x"
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

TESTCASE "dp2add - sat (16) dp2add_pp r0, r4, v1, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.423828125, -3.423828125, -3.423828125, -3.423828125
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (17) dp2add_sat_pp r0, t0, r5, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (18) dp2add_sat_pp r0, c4, t1, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (19) dp2add_sat_pp r0, r4, c5, t2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (20) dp2add_sat_pp r0, v0, c5, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (21) dp2add_sat_pp r0, t0, t0, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (22) dp2add_sat_pp r0, c4, v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (23) dp2add_sat_pp r0, t0, r5, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (24) dp2add_sat_pp r0, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - sat (25) dp2add_sat_pp r0, t0, t0, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_sat_pp r0, t0, t0, t0.x
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

TESTCASE "dp2add - Pred/Mask (1) dp2add r0, v0, c5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.3330078125, -3.3330078125, -3.3330078125, -3.3330078125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (2) dp2add r0, r4, v1, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.423828125, -3.423828125, -3.423828125, -3.423828125
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (3) dp2add r0, c4, c4, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.798828125, 5.798828125, 5.798828125, 5.798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (4) dp2add r0, t0, r5, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (5) dp2add r0, c4, t1, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.447265625, 1.447265625, 1.447265625, 1.447265625
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (6) dp2add r0, r4, c5, t2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (7) dp2add r0, v0, c5, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.7236328125, -0.7236328125, -0.7236328125, -0.7236328125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (8) dp2add r0, t0, t0, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.189453125, 3.189453125, 3.189453125, 3.189453125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (9) dp2add r0, c4, v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.908203125, -0.908203125, -0.908203125, -0.908203125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (10) dp2add r0, t0, r5, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.447265625, 1.447265625, 1.447265625, 1.447265625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (11) dp2add r0, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.18798828125, 2.18798828125, 2.18798828125, 2.18798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (12) dp2add r0, t0, t0, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.845703125, 2.845703125, 2.845703125, 2.845703125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (13) dp2add r0.x, r4, r5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.x, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (14) dp2add r0.x, v0, c5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.3330078125, 0.0, 0.0, 0.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.x, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (15) dp2add r0.x, r4, v1, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.423828125, 0.0, 0.0, 0.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.x, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (16) dp2add r0.x, c4, c4, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.798828125, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.x, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (17) dp2add r0.x, t0, r5, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.x, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (18) dp2add r0.x, c4, t1, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.447265625, 0.0, 0.0, 0.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.x, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (19) dp2add r0.x, r4, c5, t2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 0.0, 0.0, 0.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.x, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (20) dp2add r0.x, v0, c5, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.7236328125, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.x, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (21) dp2add r0.x, t0, t0, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.189453125, 0.0, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.x, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (22) dp2add r0.x, c4, v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.908203125, 0.0, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.x, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (23) dp2add r0.x, t0, r5, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.447265625, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.x, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (24) dp2add r0.x, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.18798828125, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.x, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (25) dp2add r0.x, t0, t0, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.845703125, 0.0, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.x, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (26) dp2add r0.y, r4, r5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.791015625, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.y, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (27) dp2add r0.y, v0, c5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -3.3330078125, 0.0, 0.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.y, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (28) dp2add r0.y, r4, v1, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -3.423828125, 0.0, 0.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.y, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (29) dp2add r0.y, c4, c4, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 5.798828125, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.y, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (30) dp2add r0.y, t0, r5, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.791015625, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.y, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (31) dp2add r0.y, c4, t1, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.447265625, 0.0, 0.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.y, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (32) dp2add r0.y, r4, c5, t2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.791015625, 0.0, 0.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.y, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (33) dp2add r0.y, v0, c5, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.7236328125, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.y, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (34) dp2add r0.y, t0, t0, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 3.189453125, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.y, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (35) dp2add r0.y, c4, v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.908203125, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.y, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (36) dp2add r0.y, t0, r5, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.447265625, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.y, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (37) dp2add r0.y, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 2.18798828125, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.y, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (38) dp2add r0.y, t0, t0, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 2.845703125, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.y, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (39) dp2add r0.z, r4, r5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.791015625, 0.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.z, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (40) dp2add r0.z, v0, c5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -3.3330078125, 0.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.z, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (41) dp2add r0.z, r4, v1, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -3.423828125, 0.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.z, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (42) dp2add r0.z, c4, c4, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 5.798828125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.z, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (43) dp2add r0.z, t0, r5, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.791015625, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.z, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (44) dp2add r0.z, c4, t1, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.447265625, 0.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.z, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (45) dp2add r0.z, r4, c5, t2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.791015625, 0.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.z, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (46) dp2add r0.z, v0, c5, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -0.7236328125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.z, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (47) dp2add r0.z, t0, t0, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 3.189453125, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.z, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (48) dp2add r0.z, c4, v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -0.908203125, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.z, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (49) dp2add r0.z, t0, r5, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.447265625, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.z, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (50) dp2add r0.z, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 2.18798828125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.z, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (51) dp2add r0.z, t0, t0, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 2.845703125, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.z, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (52) dp2add r0.w, r4, r5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.791015625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.w, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (53) dp2add r0.w, v0, c5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -3.3330078125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.w, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (54) dp2add r0.w, r4, v1, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -3.423828125
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.w, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (55) dp2add r0.w, c4, c4, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 5.798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.w, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (56) dp2add r0.w, t0, r5, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.791015625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.w, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (57) dp2add r0.w, c4, t1, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.447265625
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.w, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (58) dp2add r0.w, r4, c5, t2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.791015625
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.w, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (59) dp2add r0.w, v0, c5, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -0.7236328125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.w, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (60) dp2add r0.w, t0, t0, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 3.189453125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.w, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (61) dp2add r0.w, c4, v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -0.908203125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.w, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (62) dp2add r0.w, t0, r5, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.447265625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.w, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (63) dp2add r0.w, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 2.18798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.w, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (64) dp2add r0.w, t0, t0, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 2.845703125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.w, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (65) dp2add r0.xz, r4, r5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 0.0, 1.791015625, 0.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xz, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (66) dp2add r0.xz, v0, c5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.3330078125, 0.0, -3.3330078125, 0.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xz, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (67) dp2add r0.xz, r4, v1, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.423828125, 0.0, -3.423828125, 0.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xz, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (68) dp2add r0.xz, c4, c4, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.798828125, 0.0, 5.798828125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xz, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (69) dp2add r0.xz, t0, r5, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 0.0, 1.791015625, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xz, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (70) dp2add r0.xz, c4, t1, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.447265625, 0.0, 1.447265625, 0.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xz, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (71) dp2add r0.xz, r4, c5, t2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 0.0, 1.791015625, 0.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xz, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (72) dp2add r0.xz, v0, c5, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.7236328125, 0.0, -0.7236328125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xz, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (73) dp2add r0.xz, t0, t0, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.189453125, 0.0, 3.189453125, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xz, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (74) dp2add r0.xz, c4, v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.908203125, 0.0, -0.908203125, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xz, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (75) dp2add r0.xz, t0, r5, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.447265625, 0.0, 1.447265625, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xz, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (76) dp2add r0.xz, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.18798828125, 0.0, 2.18798828125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xz, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (77) dp2add r0.xz, t0, t0, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.845703125, 0.0, 2.845703125, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xz, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (78) dp2add r0.yw, r4, r5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.791015625, 0.0, 1.791015625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.yw, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (79) dp2add r0.yw, v0, c5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -3.3330078125, 0.0, -3.3330078125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.yw, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (80) dp2add r0.yw, r4, v1, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -3.423828125, 0.0, -3.423828125
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.yw, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (81) dp2add r0.yw, c4, c4, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 5.798828125, 0.0, 5.798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.yw, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (82) dp2add r0.yw, t0, r5, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.791015625, 0.0, 1.791015625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.yw, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (83) dp2add r0.yw, c4, t1, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.447265625, 0.0, 1.447265625
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.yw, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (84) dp2add r0.yw, r4, c5, t2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.791015625, 0.0, 1.791015625
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.yw, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (85) dp2add r0.yw, v0, c5, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.7236328125, 0.0, -0.7236328125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.yw, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (86) dp2add r0.yw, t0, t0, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 3.189453125, 0.0, 3.189453125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.yw, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (87) dp2add r0.yw, c4, v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.908203125, 0.0, -0.908203125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.yw, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (88) dp2add r0.yw, t0, r5, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.447265625, 0.0, 1.447265625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.yw, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (89) dp2add r0.yw, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 2.18798828125, 0.0, 2.18798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.yw, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (90) dp2add r0.yw, t0, t0, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 2.845703125, 0.0, 2.845703125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.yw, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (91) dp2add r0.xyw, r4, r5, r6.x"
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

TESTCASE "dp2add - Pred/Mask (92) dp2add r0.xyw, v0, c5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.3330078125, -3.3330078125, 0.0, -3.3330078125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xyw, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (93) dp2add r0.xyw, r4, v1, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.423828125, -3.423828125, 0.0, -3.423828125
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xyw, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (94) dp2add r0.xyw, c4, c4, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.798828125, 5.798828125, 0.0, 5.798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xyw, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (95) dp2add r0.xyw, t0, r5, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 0.0, 1.791015625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyw, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (96) dp2add r0.xyw, c4, t1, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.447265625, 1.447265625, 0.0, 1.447265625
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyw, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (97) dp2add r0.xyw, r4, c5, t2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 0.0, 1.791015625
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyw, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (98) dp2add r0.xyw, v0, c5, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.7236328125, -0.7236328125, 0.0, -0.7236328125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xyw, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (99) dp2add r0.xyw, t0, t0, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.189453125, 3.189453125, 0.0, 3.189453125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyw, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (100) dp2add r0.xyw, c4, v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.908203125, -0.908203125, 0.0, -0.908203125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xyw, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (101) dp2add r0.xyw, t0, r5, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.447265625, 1.447265625, 0.0, 1.447265625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyw, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (102) dp2add r0.xyw, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.18798828125, 2.18798828125, 0.0, 2.18798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xyw, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (103) dp2add r0.xyw, t0, t0, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.845703125, 2.845703125, 0.0, 2.845703125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyw, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (104) dp2add r0.xyzw, r4, r5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyzw, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (105) dp2add r0.xyzw, v0, c5, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.3330078125, -3.3330078125, -3.3330078125, -3.3330078125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xyzw, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (106) dp2add r0.xyzw, r4, v1, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.423828125, -3.423828125, -3.423828125, -3.423828125
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xyzw, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (107) dp2add r0.xyzw, c4, c4, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.798828125, 5.798828125, 5.798828125, 5.798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xyzw, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (108) dp2add r0.xyzw, t0, r5, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyzw, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (109) dp2add r0.xyzw, c4, t1, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.447265625, 1.447265625, 1.447265625, 1.447265625
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyzw, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (110) dp2add r0.xyzw, r4, c5, t2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyzw, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (111) dp2add r0.xyzw, v0, c5, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.7236328125, -0.7236328125, -0.7236328125, -0.7236328125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xyzw, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (112) dp2add r0.xyzw, t0, t0, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.189453125, 3.189453125, 3.189453125, 3.189453125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyzw, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (113) dp2add r0.xyzw, c4, v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.908203125, -0.908203125, -0.908203125, -0.908203125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xyzw, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (114) dp2add r0.xyzw, t0, r5, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.447265625, 1.447265625, 1.447265625, 1.447265625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyzw, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (115) dp2add r0.xyzw, v0, v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.18798828125, 2.18798828125, 2.18798828125, 2.18798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0.xyzw, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (116) dp2add r0.xyzw, t0, t0, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.845703125, 2.845703125, 2.845703125, 2.845703125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0.xyzw, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  195 tests generated.