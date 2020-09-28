// Tests for Instruction: lrp
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
TESTCASE "lrp - Standard (0) lrp r0, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3173828125, -2.982421875, 2.470703125, 3.349609375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (1) lrp_pp r0, -v0.x, c5.x, -r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.97802734375, 4.97802734375, 4.97802734375, 4.97802734375
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -v0.x, c5.x, -r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (2) lrp r0, r4.y, -v1.y, -c6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.93701171875, 2.93701171875, 2.93701171875, 2.93701171875
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, r4.y, -v1.y, -c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (3) lrp_pp r0, -c4.zxyw, -c4.zxyw, v0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.267578125, 2.92236328125, 0.95361328125, 5.833984375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4.zxyw, -c4.zxyw, v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (4) lrp_pp r0, t0, -r5.x, c6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -7.8671875, -4.6171875, -6.953125, 4.421875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, t0, -r5.x, c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (5) lrp r0, c4.x, -t1, -c4.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.5029296875, 6.029296875, 1.953125, -2.7353515625
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, c4.x, -t1, -c4.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (6) lrp r0, -r4.y, c5.zxyw, -t2"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.4033203125, -1.6796875, 0.0712890625, 1.3369140625
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -r4.y, c5.zxyw, -t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (7) lrp r0, v0.zxyw, c5.y, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.23291015625, 0.107666015625, 0.552978515625, 0.17724609375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, v0.zxyw, c5.y, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (8) lrp_pp r0, -t0, -t0.y, -c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.3330078125, 0.984375, 2.064453125, 1.408203125
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -t0, -t0.y, -c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (9) lrp_pp r0, -c4.y, -v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.8349609375, -0.017578125, -0.19921875, -0.623046875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4.y, -v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (10) lrp r0, t0.x, r5.zxyw, t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.953125, 0.630859375, -3.060546875, -5.521484375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0.x, r5.zxyw, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (11) lrp r0, v0.zxyw, v0.x, -v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.6767578125, 0.96142578125, 0.041748046875, 0.80712890625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, v0.zxyw, v0.x, -v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (12) lrp r0, t0, t0.zxyw, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.5224609375, -1.96875, -3.65625, 4.884765625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0, t0.zxyw, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (13) lrp_pp r0, -r4.x, -r5.y, -r6.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.4208984375, -1.7587890625, -1.607421875, -1.2744140625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4.x, -r5.y, -r6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (14) lrp r0, v0.y, -c5, r6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.10546875, -0.802734375, -0.5302734375, -1.408203125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, v0.y, -c5, r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (15) lrp r0, c4.y, c4.x, -v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.95361328125, 0.95361328125, 0.95361328125, 0.95361328125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, c4.y, c4.x, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (16) lrp_pp r0, -r4.zxyw, v1, c6.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0732421875, 1.66650390625, 0.18115234375, -4.3818359375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4.zxyw, v1, c6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (17) lrp_pp r0, -c4, t1.x, c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.568359375, -1.755859375, -2.33984375, 0.50390625
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4, t1.x, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (18) lrp_pp r0, r4, -c5.x, t2.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.6513671875, -4.9248046875, -6.953125, 4.142578125
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, r4, -c5.x, t2.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (19) lrp r0, -t0.x, r5, -c6"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.0810546875, -1.23828125, -0.3173828125, 1.5556640625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -t0.x, r5, -c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (20) lrp_pp r0, -v0.y, -c5, -v0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.115234375, -1.41577148438, -0.946044921875, -0.6943359375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -v0.y, -c5, -v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (21) lrp r0, t0.zxyw, t0, c6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.625, -2.453125, -1.2568359375, 4.478515625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0.zxyw, t0, c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (22) lrp r0, c4, v1.x, -v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.3974609375, -0.955078125, -2.1328125, 2.16015625
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, c4, v1.x, -v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (23) lrp_pp r0, -t0, -r5, -t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.599609375, -0.060546875, -0.369140625, -2.662109375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -t0, -r5, -t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (24) lrp_pp r0, -v0, -v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.92236328125, 1.69580078125, 2.52685546875, 2.69775390625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -v0, -v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (25) lrp_pp r0, -t0.x, -t0, -t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.96875, 0.96875, 1.6875, -1.8125
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -t0.x, -t0, -t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (26) lrp r0, v0, c5.y, -r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.11767578125, 0.802734375, -0.0185546875, 0.1904296875
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, v0, c5.y, -r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (27) lrp_pp r0, -r4, v1.x, -c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.17919921875, 0.90869140625, 1.9326171875, 1.5498046875
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4, v1.x, -c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (28) lrp_pp r0, c4, -c4, -v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -6.79833984375, -1.89208984375, -5.115234375, -2.548828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, c4, -c4, -v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (29) lrp_pp r0, -v0, -c5.x, -v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.70678710938, -1.58178710938, -3.05859375, -3.341796875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -v0, -c5.x, -v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (30) lrp r0, r4.x, c5, -t2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 8.3310546875, 4.6396484375, 5.7470703125, 2.1787109375
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, r4.x, c5, -t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (31) lrp r0, -t0.x, t0.x, -c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.4501953125, -5.4501953125, -5.4501953125, -5.4501953125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -t0.x, t0.x, -c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (32) lrp_pp r0, c4.x, -v1.y, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.2314453125, 0.2314453125, 0.2314453125, 0.2314453125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, c4.x, -v1.y, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (33) lrp r0, -v0.x, v0.y, -v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.43798828125, -1.43798828125, -1.43798828125, -1.43798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, -v0.x, v0.y, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (34) lrp r0, r4.y, -r5.x, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.9248046875, -4.9248046875, -4.9248046875, -4.9248046875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, r4.y, -r5.x, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (35) lrp r0, -t0.y, -r5.y, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.1416015625, -0.1416015625, -0.1416015625, -0.1416015625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -t0.y, -r5.y, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (36) lrp_pp r0, c4.y, t1.y, -c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.78515625, 1.81640625, 3.2314453125, -3.6591796875
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, c4.y, t1.y, -c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (37) lrp r0, -t0.y, -r5.x, -t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.787109375, 1.787109375, 1.787109375, 1.787109375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -t0.y, -r5.x, -t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (38) lrp_pp r0, -t0.y, -t0.x, -t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.9375, 1.9375, 1.9375, 1.9375
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -t0.y, -t0.x, -t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (39) lrp r0, -r4.x, -v1.zxyw, -c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.03564453125, -3.32763671875, -1.66650390625, -2.89697265625
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, -r4.x, -v1.zxyw, -c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (40) lrp r0, -c4.x, c4.y, -v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.95361328125, -0.95361328125, -0.95361328125, -0.95361328125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, -c4.x, c4.y, -v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (41) lrp r0, -v0.x, c5.zxyw, -v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.499755859375, 0.792236328125, -1.05346679688, -2.28393554688
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, -v0.x, c5.zxyw, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (42) lrp_pp r0, t0.y, -t0.zxyw, -c6.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.865234375, 1.2919921875, 1.953125, 3.970703125
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, t0.y, -t0.zxyw, -c6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (43) lrp r0, -v0.y, -v0.zxyw, v0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.6611328125, 1.93798828125, 0.95361328125, 1.7841796875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, -v0.y, -v0.zxyw, v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (44) lrp_pp r0, -r4.zxyw, r5.zxyw, -r6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.80078125, -4.9296875, 0.13671875, 1.6953125
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4.zxyw, r5.zxyw, -r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (45) lrp r0, v0.zxyw, -c5.zxyw, -r6.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.2978515625, 1.77880859375, 0.7119140625, -1.1123046875
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, v0.zxyw, -c5.zxyw, -r6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (46) lrp r0, -t0.zxyw, -r5.zxyw, -c6.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.220703125, 1.9326171875, -0.044921875, 5.599609375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -t0.zxyw, -r5.zxyw, -c6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (47) lrp r0, c4.zxyw, t1.zxyw, -c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 6.08203125, 9.3515625, 3.78515625, 0.8359375
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, c4.zxyw, t1.zxyw, -c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (48) lrp r0, r4.zxyw, c5.y, t2.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.10546875, -4.544921875, -2.982421875, 1.36328125
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, r4.zxyw, c5.y, t2.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (49) lrp_pp r0, c4.zxyw, v1.zxyw, v1.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.234375, 0.890625, 0.046875, 0.671875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, c4.zxyw, v1.zxyw, v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (50) lrp r0, t0.zxyw, r5.y, -t0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.376953125, 5.66015625, 1.81640625, 1.642578125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0.zxyw, r5.y, -t0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (51) lrp_pp r0, -t0.zxyw, -t0.y, t0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.794921875, 3.814453125, 0.908203125, 3.341796875
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -t0.zxyw, -t0.y, t0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * sat * pp
TESTCASE "lrp - sat (0) lrp r0, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3173828125, -2.982421875, 2.470703125, 3.349609375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (1) lrp_pp r0, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.77880859375, -0.7119140625, -0.2978515625, 1.1123046875
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (2) lrp_sat r0, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (3) lrp_sat_pp r0, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (4) lrp r0, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3173828125, -2.982421875, 2.470703125, 3.349609375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (5) lrp r0, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.337890625, -1.998046875, -3.744140625, 0.962890625
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (6) lrp r0, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3173828125, -2.982421875, 2.470703125, 3.349609375
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (7) lrp r0, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.73803710938, 0.295166015625, -0.263671875, 1.302734375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (8) lrp r0, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.9482421875, -1.953125, 4.52734375, 4.19921875
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (9) lrp r0, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.890625, 0.046875, 0.234375, 0.671875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (10) lrp r0, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.337890625, -1.998046875, -3.744140625, 0.962890625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (11) lrp r0, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.984375, 0.484375, 0.84375, 0.90625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (12) lrp r0, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.96875, -0.96875, -1.6875, 1.8125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (13) lrp_sat r0, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (14) lrp r0, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 6.79833984375, 1.89208984375, 5.115234375, 2.548828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (15) lrp_sat_pp r0, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (16) lrp_pp r0, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, -6.57763671875, -2.93701171875, 1.2841796875, 2.1318359375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (17) lrp_sat_pp r0, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (18) lrp_sat_pp r0, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.962890625
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (19) lrp_sat_pp r0, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (20) lrp_sat_pp r0, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.295166015625, 0.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (21) lrp_sat_pp r0, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (22) lrp_sat_pp r0, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.890625, 0.046875, 0.234375, 0.671875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (23) lrp_sat_pp r0, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.962890625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (24) lrp_sat_pp r0, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.984375, 0.484375, 0.84375, 0.90625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (25) lrp_sat_pp r0, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// masks & predication
TESTCASE "lrp - Pred/Mask (0) lrp r0, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3173828125, -2.982421875, 2.470703125, 3.349609375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (1) lrp_pp r0.x, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.77880859375, 0.0, 0.0, 0.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.x, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (2) lrp_sat r0.y, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.y, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (3) lrp_sat_pp r0.z, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.z, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (4) lrp r0.w, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 3.349609375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.w, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (5) lrp r0.xz, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.337890625, 0.0, -3.744140625, 0.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.xz, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (6) lrp r0.yw, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -2.982421875, 0.0, 3.349609375
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.yw, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (7) lrp r0.xyw, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.73803710938, 0.295166015625, 0.0, 1.302734375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0.xyw, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (8) lrp r0.xyzw, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.9482421875, -1.953125, 4.52734375, 4.19921875
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.xyzw, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (9) lrp_sat_pp r0, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.890625, 0.046875, 0.234375, 0.671875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (10) lrp_sat r0.x, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.x, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (11) lrp_pp r0.y, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.484375, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.y, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (12) lrp r0.z, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -1.6875, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.z, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (13) lrp_sat_pp r0.w, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.w, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (14) lrp_sat r0.xz, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.xz, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (15) lrp_pp r0.yw, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -2.93701171875, 0.0, 2.1318359375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.yw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (16) lrp r0.xyw, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 6.79833984375, 1.89208984375, 0.0, 2.548828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0.xyw, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (17) lrp_sat_pp r0.xyw, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.xyw, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (18) lrp_sat_pp r0.xyzw, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.962890625
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.xyzw, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (19) lrp_sat_pp r0.xz, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.xz, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (20) lrp_sat_pp r0.yw, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.295166015625, 0.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.yw, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (21) lrp_sat_pp r0, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (22) lrp_pp r0, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.337890625, -1.998046875, -3.744140625, 0.962890625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (23) lrp_sat r0, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.984375, 0.484375, 0.84375, 0.90625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (24) lrp_sat_pp r0, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (25) lrp r0.x, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.890625, 0.0, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp r0.x, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (26) lrp_sat r0, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (27) lrp_sat_pp r0, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (28) lrp r0, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 6.79833984375, 1.89208984375, 5.115234375, 2.548828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (29) lrp_sat_pp r0, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (30) lrp r0, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.337890625, -1.998046875, -3.744140625, 0.962890625
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (31) lrp r0, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3173828125, -2.982421875, 2.470703125, 3.349609375
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (32) lrp_sat r0, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.295166015625, 0.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (33) lrp r0.x, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3173828125, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.x, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (34) lrp_sat r0.x, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.x, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (35) lrp_sat_pp r0.x, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.x, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (36) lrp_sat r0.x, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.x, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (37) lrp_sat_pp r0.x, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.x, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (38) lrp_pp r0.x, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3173828125, 0.0, 0.0, 0.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.x, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (39) lrp_pp r0.x, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.73803710938, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.x, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (40) lrp_sat r0.x, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.x, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (41) lrp_pp r0.x, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.984375, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.x, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (42) lrp_pp r0.x, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.96875, 0.0, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.x, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (43) lrp_sat_pp r0.y, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.y, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (44) lrp_sat r0.y, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.y, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (45) lrp_pp r0.y, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.89208984375, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.y, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (46) lrp_sat_pp r0.y, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.y, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (47) lrp r0.y, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.998046875, 0.0, 0.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.y, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (48) lrp r0.y, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -2.982421875, 0.0, 0.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.y, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (49) lrp_sat r0.y, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.295166015625, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.y, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (50) lrp_sat_pp r0.y, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.y, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (51) lrp_sat_pp r0.y, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.046875, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.y, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (52) lrp_pp r0.y, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.998046875, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.y, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (53) lrp r0.y, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.96875, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.y, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (54) lrp_sat r0.z, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.z, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (55) lrp_sat r0.z, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.z, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (56) lrp_sat_pp r0.z, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.z, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (57) lrp_sat r0.z, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.z, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (58) lrp_sat_pp r0.z, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.z, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (59) lrp_sat r0.z, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.z, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (60) lrp_sat_pp r0.z, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.z, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (61) lrp_sat_pp r0.z, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.z, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (62) lrp_pp r0.z, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.234375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.z, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (63) lrp_sat_pp r0.z, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.z, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (64) lrp_sat r0.z, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.84375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.z, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (65) lrp_sat_pp r0.w, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.w, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (66) lrp_sat_pp r0.w, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.w, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (67) lrp r0.w, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 2.548828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0.w, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (68) lrp_sat_pp r0.w, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.962890625
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.w, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (69) lrp_pp r0.w, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 3.349609375
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.w, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (70) lrp_sat_pp r0.w, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.w, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (71) lrp_sat_pp r0.w, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.w, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (72) lrp_pp r0.w, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.671875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.w, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (73) lrp r0.w, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.962890625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.w, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (74) lrp r0.w, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.90625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0.w, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (75) lrp r0.w, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.8125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.w, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (76) lrp_pp r0.xz, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3173828125, 0.0, 2.470703125, 0.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.xz, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (77) lrp_sat_pp r0.xz, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.xz, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (78) lrp_pp r0.xz, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 6.79833984375, 0.0, 5.115234375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.xz, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (79) lrp_sat r0.xz, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.xz, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (80) lrp_sat_pp r0.xz, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.xz, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (81) lrp r0.xz, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.9482421875, 0.0, 4.52734375, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.xz, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (82) lrp_sat_pp r0.xz, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.890625, 0.0, 0.234375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.xz, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (83) lrp_sat r0.xz, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.xz, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (84) lrp r0.xz, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.984375, 0.0, 0.84375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0.xz, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (85) lrp_sat r0.xz, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.xz, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (86) lrp_sat r0.yw, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.yw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (87) lrp_pp r0.yw, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.7119140625, 0.0, 1.1123046875
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.yw, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (88) lrp_pp r0.yw, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.89208984375, 0.0, 2.548828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.yw, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (89) lrp r0.yw, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -2.982421875, 0.0, 3.349609375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.yw, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (90) lrp_pp r0.yw, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.998046875, 0.0, 0.962890625
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.yw, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (91) lrp_sat r0.yw, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.yw, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (92) lrp_sat r0.yw, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.046875, 0.0, 0.671875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.yw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (93) lrp_sat r0.yw, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.962890625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.yw, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (94) lrp r0.yw, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.484375, 0.0, 0.90625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0.yw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (95) lrp r0.yw, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.96875, 0.0, 1.8125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.yw, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (96) lrp_sat r0.xyw, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.xyw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (97) lrp_sat r0.xyw, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.xyw, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (98) lrp_sat r0.xyw, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.xyw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (99) lrp r0.xyw, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.337890625, -1.998046875, 0.0, 0.962890625
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.xyw, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (100) lrp_pp r0.xyw, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3173828125, -2.982421875, 0.0, 3.349609375
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.xyw, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (101) lrp_sat r0.xyw, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.xyw, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (102) lrp r0.xyw, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.890625, 0.046875, 0.0, 0.671875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp r0.xyw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (103) lrp_pp r0.xyw, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.337890625, -1.998046875, 0.0, 0.962890625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.xyw, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (104) lrp_sat r0.xyw, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.984375, 0.484375, 0.0, 0.90625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.xyw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (105) lrp_sat_pp r0.xyw, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.xyw, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (106) lrp_pp r0.xyzw, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3173828125, -2.982421875, 2.470703125, 3.349609375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.xyzw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (107) lrp_sat r0.xyzw, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.xyzw, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (108) lrp_pp r0.xyzw, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, -6.57763671875, -2.93701171875, 1.2841796875, 2.1318359375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.xyzw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (109) lrp r0.xyzw, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 6.79833984375, 1.89208984375, 5.115234375, 2.548828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0.xyzw, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (110) lrp r0.xyzw, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3173828125, -2.982421875, 2.470703125, 3.349609375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.xyzw, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (111) lrp_sat_pp r0.xyzw, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.xyzw, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (112) lrp r0.xyzw, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.73803710938, 0.295166015625, -0.263671875, 1.302734375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0.xyzw, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (113) lrp r0.xyzw, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.890625, 0.046875, 0.234375, 0.671875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp r0.xyzw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (114) lrp_sat r0.xyzw, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.962890625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.xyzw, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (115) lrp_sat_pp r0.xyzw, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.984375, 0.484375, 0.84375, 0.90625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.xyzw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (116) lrp r0.xyzw, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.96875, -0.96875, -1.6875, 1.8125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.xyzw, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  195 tests generated.