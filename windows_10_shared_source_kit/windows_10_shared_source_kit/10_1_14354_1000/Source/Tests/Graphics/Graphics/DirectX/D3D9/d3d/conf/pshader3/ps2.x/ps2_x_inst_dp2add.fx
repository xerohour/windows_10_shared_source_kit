// Tests for Instruction: dp2add
#include "include.fx"

#define PS_SETUP \
	ps_2_x
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

#define USEPREDICATE bool UsePredicate = true;
#define USEARBSWZ bool UseArbSwizzle = true;
#define TESTCASE \
Technique \
< \
	String Shape = "TinyQuad"; \
	String Name = 
#define PASSBEGIN \
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
TESTCASE "dp2add - Standard (0) dp2add r0, r4, r5, r6.x";
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

TESTCASE "dp2add - Standard (1) dp2add_pp r0, -v0.x, c5.x, -r6.y";
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

TESTCASE "dp2add - Standard (2) dp2add r0, r4.y, -v1.y, -c6.z";
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

TESTCASE "dp2add - Standard (3) dp2add_pp r0, -c4.zxyw, -c4.zxyw, v0.w";
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

TESTCASE "dp2add - Standard (4) dp2add_pp r0, t0.xw, -r5.xw, -c6.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -4.3173828125, -4.3173828125, -4.3173828125, -4.3173828125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, t0.xw, -r5.xw, -c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (5) dp2add r0, c4.wzx, -t1.wzx, c4.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -4.1953125, -4.1953125, -4.1953125, -4.1953125
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4.wzx, -t1.wzx, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (6) dp2add r0, -r4.yzwy, c5.yzwy, t2.z";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.0751953125, -0.0751953125, -0.0751953125, -0.0751953125
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -r4.yzwy, c5.yzwy, t2.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (7) dp2add r0, v0, -c5.x, -v0.w";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.7099609375, 1.7099609375, 1.7099609375, 1.7099609375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0, -c5.x, -v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (8) dp2add_pp r0, t0.x, -t0, c6.z";
	PASSBEGIN
	TESTBEGIN
	def c1, -5.158203125, -5.158203125, -5.158203125, -5.158203125
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, t0.x, -t0, c6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (9) dp2add r0, -c4.y, v1.zxyw, v1.x";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.98046875, 1.98046875, 1.98046875, 1.98046875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -c4.y, v1.zxyw, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (10) dp2add r0, t0.zxyw, r5.y, t0.x";
	PASSBEGIN
	TESTBEGIN
	def c1, -2.3115234375, -2.3115234375, -2.3115234375, -2.3115234375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0.zxyw, r5.y, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (11) dp2add r0, -v0.xw, v0.wzx, v0.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.75048828125, -0.75048828125, -0.75048828125, -0.75048828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -v0.xw, v0.wzx, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (12) dp2add r0, -t0.wzx, t0.xw, t0.z";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 4.939453125, 4.939453125, 4.939453125, 4.939453125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -t0.wzx, t0.xw, t0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (13) dp2add_pp r0, -r4.yzwy, -r5.yzwy, -r6.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.1689453125, 2.1689453125, 2.1689453125, 2.1689453125
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4.yzwy, -r5.yzwy, -r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (14) dp2add r0, v0, -c5.y, r6.z";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.4873046875, 0.4873046875, 0.4873046875, 0.4873046875
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0, -c5.y, r6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (15) dp2add_pp r0, -r4.wzx, v1, c6.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.66015625, -2.66015625, -2.66015625, -2.66015625
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4.wzx, v1, c6.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (16) dp2add r0, c4.yzwy, c4.y, -v0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.5888671875, 1.5888671875, 1.5888671875, 1.5888671875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, c4.yzwy, c4.y, -v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (17) dp2add r0, -t0.x, r5.y, c6.w";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.755859375, -0.755859375, -0.755859375, -0.755859375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -t0.x, r5.y, c6.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (18) dp2add_pp r0, -c4.y, t1.x, -c4.x";
	PASSBEGIN
	TESTBEGIN
	def c1, -1.482421875, -1.482421875, -1.482421875, -1.482421875
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -c4.y, t1.x, -c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (19) dp2add_pp r0, r4.zxyw, -c5.wzx, -t2.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.9697265625, 2.9697265625, 2.9697265625, 2.9697265625
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, r4.zxyw, -c5.wzx, -t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (20) dp2add r0, -t0, t0.zxyw, -c6.y";
	PASSBEGIN
	TESTBEGIN
	def c1, -3.7607421875, -3.7607421875, -3.7607421875, -3.7607421875
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -t0, t0.zxyw, -c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (21) dp2add_pp r0, -v0.xw, c5.yzwy, v0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.31689453125, 1.31689453125, 1.31689453125, 1.31689453125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -v0.xw, c5.yzwy, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (22) dp2add_pp r0, -t0.y, -r5, -t0.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 2.603515625, 2.603515625, 2.603515625, 2.603515625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -t0.y, -r5, -t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (23) dp2add_pp r0, c4.zxyw, -v1.x, -v1.z";
	PASSBEGIN
	TESTBEGIN
	def c1, 3.02197265625, 3.02197265625, 3.02197265625, 3.02197265625
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4.zxyw, -v1.x, -v1.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (24) dp2add_pp r0, t0.xw, -t0.y, -t0.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.8173828125, 0.8173828125, 0.8173828125, 0.8173828125
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, t0.xw, -t0.y, -t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (25) dp2add_pp r0, v0.wzx, -v0.zxyw, -v0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.57958984375, -2.57958984375, -2.57958984375, -2.57958984375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, v0.wzx, -v0.zxyw, -v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (26) dp2add r0, v0.yzwy, c5.xw, -r6.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.39599609375, 1.39599609375, 1.39599609375, 1.39599609375
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0.yzwy, c5.xw, -r6.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (27) dp2add_pp r0, -r4.x, r5.zxyw, -r6.z";
	PASSBEGIN
	TESTBEGIN
	def c1, -5.0546875, -5.0546875, -5.0546875, -5.0546875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4.x, r5.zxyw, -r6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (28) dp2add_pp r0, r4, -v1.yzwy, -c6.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.9443359375, 1.9443359375, 1.9443359375, 1.9443359375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, r4, -v1.yzwy, -c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (29) dp2add r0, -c4.x, -c4.xw, -v0.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.1767578125, -0.1767578125, -0.1767578125, -0.1767578125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -c4.x, -c4.xw, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (30) dp2add_pp r0, t0.y, -r5.wzx, -c6.z";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.22265625, 0.22265625, 0.22265625, 0.22265625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, t0.y, -r5.wzx, -c6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (31) dp2add r0, c4.zxyw, -t1.yzwy, c4.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0478515625, 1.0478515625, 1.0478515625, 1.0478515625
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4.zxyw, -t1.yzwy, c4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (32) dp2add_pp r0, r4.xw, -c5.x, t2.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.7470703125, -1.7470703125, -1.7470703125, -1.7470703125
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, r4.xw, -c5.x, t2.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (33) dp2add r0, -v0.yzwy, c5, v0.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.26806640625, 1.26806640625, 1.26806640625, 1.26806640625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -v0.yzwy, c5, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (34) dp2add_pp r0, c4.x, v1.wzx, -v1.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.8310546875, -1.8310546875, -1.8310546875, -1.8310546875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4.x, v1.wzx, -v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (35) dp2add r0, -t0.y, t0.xw, c6.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.2763671875, -1.2763671875, -1.2763671875, -1.2763671875
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -t0.y, t0.xw, c6.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (36) dp2add r0, -v0.zxyw, -v0, -v0.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.822998046875, 0.822998046875, 0.822998046875, 0.822998046875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -v0.zxyw, -v0, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (37) dp2add_pp r0, t0.xw, r5.zxyw, t0.z";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.9931640625, -3.9931640625, -3.9931640625, -3.9931640625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, t0.xw, r5.zxyw, t0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (38) dp2add_pp r0, -t0.yzwy, -t0.x, -t0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 7.1982421875, 7.1982421875, 7.1982421875, 7.1982421875
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -t0.yzwy, -t0.x, -t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (39) dp2add_pp r0, -r4.wzx, r5.x, -r6.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.34765625, 1.34765625, 1.34765625, 1.34765625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4.wzx, r5.x, -r6.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (40) dp2add_pp r0, -c4, c4.wzx, -v0.z";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.08984375, 1.08984375, 1.08984375, 1.08984375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -c4, c4.wzx, -v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (41) dp2add_pp r0, r4.x, v1.x, -c6.y";
	PASSBEGIN
	TESTBEGIN
	def c1, -2.0380859375, -2.0380859375, -2.0380859375, -2.0380859375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, r4.x, v1.x, -c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (42) dp2add_pp r0, c4.x, -t1, -c4.z";
	PASSBEGIN
	TESTBEGIN
	def c1, -1.634765625, -1.634765625, -1.634765625, -1.634765625
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4.x, -t1, -c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (43) dp2add_pp r0, -r4.x, c5, t2.w";
	PASSBEGIN
	TESTBEGIN
	def c1, -4.447265625, -4.447265625, -4.447265625, -4.447265625
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4.x, c5, t2.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (44) dp2add r0, -v0.x, -c5.y, -v0.z";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.6591796875, -0.6591796875, -0.6591796875, -0.6591796875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -v0.x, -c5.y, -v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (45) dp2add_pp r0, t0.x, r5.yzwy, -t0.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.07421875, -1.07421875, -1.07421875, -1.07421875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, t0.x, r5.yzwy, -t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (46) dp2add_pp r0, -v0.wzx, c5.yzwy, r6.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.314453125, -1.314453125, -1.314453125, -1.314453125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -v0.wzx, c5.yzwy, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (47) dp2add_pp r0, -t0.yzwy, r5.zxyw, -c6.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.9912109375, -1.9912109375, -1.9912109375, -1.9912109375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -t0.yzwy, r5.zxyw, -c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (48) dp2add_pp r0, c4, v1.xw, v1.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.732421875, -1.732421875, -1.732421875, -1.732421875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4, v1.xw, v1.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (49) dp2add r0, v0.x, -v0.x, v0.z";
	PASSBEGIN
	TESTBEGIN
	def c1, -1.09423828125, -1.09423828125, -1.09423828125, -1.09423828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0.x, -v0.x, v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (50) dp2add r0, t0.x, t0, t0.w";
	PASSBEGIN
	TESTBEGIN
	def c1, 7.595703125, 7.595703125, 7.595703125, 7.595703125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0.x, t0, t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (51) dp2add_pp r0, t0.zxyw, -t0.x, c6.x";
	PASSBEGIN
	TESTBEGIN
	def c1, -8.8232421875, -8.8232421875, -8.8232421875, -8.8232421875
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, t0.zxyw, -t0.x, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (52) dp2add r0, r4.y, -r5.y, -r6.z";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.443359375, -0.443359375, -0.443359375, -0.443359375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, r4.y, -r5.y, -r6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (53) dp2add_pp r0, v0.y, -c5, r6.z";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.4423828125, 1.4423828125, 1.4423828125, 1.4423828125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, v0.y, -c5, r6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (54) dp2add_pp r0, c4.y, c4.yzwy, v0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 3.5576171875, 3.5576171875, 3.5576171875, 3.5576171875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4.y, c4.yzwy, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (55) dp2add r0, r4.zxyw, v1.xw, c6.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.95068359375, -3.95068359375, -3.95068359375, -3.95068359375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, r4.zxyw, v1.xw, c6.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (56) dp2add_pp r0, c4.xw, -t1.y, -c4.z";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.7021484375, 1.7021484375, 1.7021484375, 1.7021484375
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4.xw, -t1.y, -c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (57) dp2add_pp r0, t0.xw, -t0.y, -c6.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.9736328125, 0.9736328125, 0.9736328125, 0.9736328125
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, t0.xw, -t0.y, -c6.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (58) dp2add r0, -c4.xw, v1, -v1.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.77783203125, 0.77783203125, 0.77783203125, 0.77783203125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -c4.xw, v1, -v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (59) dp2add_pp r0, -t0.wzx, r5, -c6.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 4.85546875, 4.85546875, 4.85546875, 4.85546875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -t0.wzx, r5, -c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (60) dp2add_pp r0, -r4.wzx, -c5.y, t2.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.11328125, -1.11328125, -1.11328125, -1.11328125
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4.wzx, -c5.y, t2.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (61) dp2add r0, t0.yzwy, r5.wzx, t0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.4794921875, -2.4794921875, -2.4794921875, -2.4794921875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0.yzwy, r5.wzx, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (62) dp2add_pp r0, -v0, -v0.y, -v0.z";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.13232421875, -0.13232421875, -0.13232421875, -0.13232421875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -v0, -v0.y, -v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (63) dp2add_pp r0, t0, t0.zxyw, t0.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 4.2607421875, 4.2607421875, 4.2607421875, 4.2607421875
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, t0, t0.zxyw, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (64) dp2add_pp r0, -v0.y, -c5.zxyw, -v0.y";
	PASSBEGIN
	TESTBEGIN
	def c1, -1.57421875, -1.57421875, -1.57421875, -1.57421875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -v0.y, -c5.zxyw, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (65) dp2add r0, -t0, r5.x, -c6.z";
	PASSBEGIN
	TESTBEGIN
	def c1, -5.857421875, -5.857421875, -5.857421875, -5.857421875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -t0, r5.x, -c6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (66) dp2add_pp r0, -c4, -t1.zxyw, -c4.z";
	PASSBEGIN
	TESTBEGIN
	def c1, 4.3359375, 4.3359375, 4.3359375, 4.3359375
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -c4, -t1.zxyw, -c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (67) dp2add_pp r0, -r4, c5.zxyw, -t2.y";
	PASSBEGIN
	TESTBEGIN
	def c1, -1.1796875, -1.1796875, -1.1796875, -1.1796875
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -r4, c5.zxyw, -t2.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (68) dp2add_pp r0, -t0, -r5.x, -t0.w";
	PASSBEGIN
	TESTBEGIN
	def c1, 3.419921875, 3.419921875, 3.419921875, 3.419921875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -t0, -r5.x, -t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (69) dp2add r0, -v0.y, -v0.xw, -v0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.068603515625, -0.068603515625, -0.068603515625, -0.068603515625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -v0.y, -v0.xw, -v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (70) dp2add r0, -t0.y, -t0.wzx, -t0.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.93359375, -1.93359375, -1.93359375, -1.93359375
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -t0.y, -t0.wzx, -t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (71) dp2add_pp r0, r4.zxyw, r5.xw, r6.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.2646484375, -1.2646484375, -1.2646484375, -1.2646484375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, r4.zxyw, r5.xw, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (72) dp2add r0, -v0.zxyw, c5.zxyw, r6.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.68017578125, 0.68017578125, 0.68017578125, 0.68017578125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -v0.zxyw, c5.zxyw, r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (73) dp2add_pp r0, v0.zxyw, c5.xw, -v0.z";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.02392578125, -1.02392578125, -1.02392578125, -1.02392578125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, v0.zxyw, c5.xw, -v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (74) dp2add_pp r0, r4.xw, v1.zxyw, -c6.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.27783203125, 2.27783203125, 2.27783203125, 2.27783203125
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, r4.xw, v1.zxyw, -c6.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (75) dp2add_pp r0, -c4.xw, c4, v0.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.2138671875, -1.2138671875, -1.2138671875, -1.2138671875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -c4.xw, c4, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (76) dp2add r0, t0.wzx, -t0.wzx, -c6.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -4.6640625, -4.6640625, -4.6640625, -4.6640625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0.wzx, -t0.wzx, -c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (77) dp2add_pp r0, c4.wzx, v1.y, -v1.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.884765625, -0.884765625, -0.884765625, -0.884765625
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4.wzx, v1.y, -v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (78) dp2add r0, r4.y, -c5.xw, t2.z";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.201171875, 0.201171875, 0.201171875, 0.201171875
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, r4.y, -c5.xw, t2.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (79) dp2add r0, -t0.zxyw, r5.yzwy, c6.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.8896484375, -1.8896484375, -1.8896484375, -1.8896484375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -t0.zxyw, r5.yzwy, c6.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (80) dp2add r0, t0.zxyw, -t0.yzwy, t0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -6.92578125, -6.92578125, -6.92578125, -6.92578125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0.zxyw, -t0.yzwy, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (81) dp2add r0, -r4.xw, r5.wzx, r6.z";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 4.1201171875, 4.1201171875, 4.1201171875, 4.1201171875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, -r4.xw, r5.wzx, r6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (82) dp2add r0, v0.xw, -c5.wzx, r6.z";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.27294921875, -0.27294921875, -0.27294921875, -0.27294921875
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0.xw, -c5.wzx, r6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (83) dp2add r0, c4.wzx, c4.x, -v0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.23046875, -1.23046875, -1.23046875, -1.23046875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, c4.wzx, c4.x, -v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (84) dp2add_pp r0, -v0.wzx, c5.wzx, -v0.w";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.728515625, -1.728515625, -1.728515625, -1.728515625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, -v0.wzx, c5.wzx, -v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (85) dp2add r0, t0.wzx, r5.xw, -t0.z";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.80859375, -3.80859375, -3.80859375, -3.80859375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, t0.wzx, r5.xw, -t0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (86) dp2add r0, -r4.yzwy, -v1.wzx, c6.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.67138671875, -2.67138671875, -2.67138671875, -2.67138671875
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -r4.yzwy, -v1.wzx, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (87) dp2add r0, c4.yzwy, t1.xw, c4.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.5107421875, -1.5107421875, -1.5107421875, -1.5107421875
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	dp2add r0, c4.yzwy, t1.xw, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (88) dp2add_pp r0, t0.yzwy, t0.yzwy, -c6.z";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 3.1611328125, 3.1611328125, 3.1611328125, 3.1611328125
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, t0.yzwy, t0.yzwy, -c6.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (89) dp2add_pp r0, c4.yzwy, v1.yzwy, -v1.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.33154296875, -1.33154296875, -1.33154296875, -1.33154296875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp2add_pp r0, c4.yzwy, v1.yzwy, -v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (90) dp2add r0, v0.yzwy, v0.yzwy, v0.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.43090820313, 1.43090820313, 1.43090820313, 1.43090820313
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, v0.yzwy, v0.yzwy, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Standard (91) dp2add r0, -v0.x, -v0.yzwy, v0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.29174804688, 2.29174804688, 2.29174804688, 2.29174804688
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp2add r0, -v0.x, -v0.yzwy, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * sat * pp
TESTCASE "dp2add - sat (0) dp2add r0, r4, r5, r6.x";
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

TESTCASE "dp2add - sat (1) dp2add_pp r0, v0, c5, r6.x";
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

TESTCASE "dp2add - sat (2) dp2add_sat r0, r4, v1, c6.x";
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

TESTCASE "dp2add - sat (3) dp2add_sat_pp r0, c4, c4, v0.x";
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

TESTCASE "dp2add - sat (4) dp2add r0, t0, r5, c6.x";
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

TESTCASE "dp2add - sat (5) dp2add r0, c4, t1, c4.x";
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

TESTCASE "dp2add - sat (6) dp2add r0, r4, c5, t2.x";
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

TESTCASE "dp2add - sat (7) dp2add r0, v0, c5, v0.x";
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

TESTCASE "dp2add - sat (8) dp2add r0, t0, t0, c6.x";
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

TESTCASE "dp2add - sat (9) dp2add r0, c4, v1, v1.x";
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

TESTCASE "dp2add - sat (10) dp2add r0, t0, r5, t0.x";
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

TESTCASE "dp2add - sat (11) dp2add r0, v0, v0, v0.x";
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

TESTCASE "dp2add - sat (12) dp2add r0, t0, t0, t0.x";
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

TESTCASE "dp2add - sat (13) dp2add_sat r0, v0, c5, r6.x";
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

TESTCASE "dp2add - sat (14) dp2add r0, c4, c4, v0.x";
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

TESTCASE "dp2add - sat (15) dp2add_sat_pp r0, r4, r5, r6.x";
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

TESTCASE "dp2add - sat (16) dp2add_pp r0, r4, v1, c6.x";
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

TESTCASE "dp2add - sat (17) dp2add_sat_pp r0, t0, r5, c6.x";
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

TESTCASE "dp2add - sat (18) dp2add_sat_pp r0, c4, t1, c4.x";
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

TESTCASE "dp2add - sat (19) dp2add_sat_pp r0, r4, c5, t2.x";
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

TESTCASE "dp2add - sat (20) dp2add_sat_pp r0, v0, c5, v0.x";
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

TESTCASE "dp2add - sat (21) dp2add_sat_pp r0, t0, t0, c6.x";
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

TESTCASE "dp2add - sat (22) dp2add_sat_pp r0, c4, v1, v1.x";
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

TESTCASE "dp2add - sat (23) dp2add_sat_pp r0, t0, r5, t0.x";
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

TESTCASE "dp2add - sat (24) dp2add_sat_pp r0, v0, v0, v0.x";
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

TESTCASE "dp2add - sat (25) dp2add_sat_pp r0, t0, t0, t0.x";
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
TESTCASE "dp2add - Pred/Mask (0) dp2add r0, r4, r5, r6.x";
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

TESTCASE "dp2add - Pred/Mask (1) (p0.x) dp2add r0.x, v0, c5, r6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -3.3330078125, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) dp2add r0.x, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (2) (p0.y) dp2add r0.y, r4, v1, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, -3.423828125, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) dp2add r0.y, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (3) (p0.z) dp2add r0.z, c4, c4, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 5.798828125, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) dp2add r0.z, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (4) (p0.w) dp2add r0.w, t0, r5, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp2add r0.w, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (5) dp2add r0.xz, c4, t1, c4.x";
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

TESTCASE "dp2add - Pred/Mask (6) (!p0) dp2add r0.yw, r4, c5, t2.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 1.791015625, 0.0, 1.791015625
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp2add r0.yw, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (7) (!p0.z) dp2add r0.xyw, v0, c5, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, -0.7236328125, -0.7236328125, 0.0, -0.7236328125
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) dp2add r0.xyw, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (8) (!p0.y) dp2add r0.xyzw, t0, t0, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) dp2add r0.xyzw, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (9) (!p0.x) dp2add r0, c4, v1, v1.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -0.908203125, -0.908203125, -0.908203125, -0.908203125
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) dp2add r0, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (10) dp2add r0.x, t0, r5, t0.x";
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

TESTCASE "dp2add - Pred/Mask (11) (!p0.z) dp2add r0.y, v0, v0, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) dp2add r0.y, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (12) (!p0) dp2add r0.z, t0, t0, t0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 2.845703125, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp2add r0.z, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (13) (!p0.x) dp2add r0.w, r4, r5, r6.x";
	USEPREDICATE
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
	(!p0.x) dp2add r0.w, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (14) (!p0) dp2add r0.xz, v0, c5, r6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -3.3330078125, 0.0, -3.3330078125, 0.0
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0) dp2add r0.xz, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (15) dp2add r0.yw, r4, v1, c6.x";
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

TESTCASE "dp2add - Pred/Mask (16) (!p0.y) dp2add r0.xyw, c4, c4, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 5.798828125, 5.798828125, 0.0, 5.798828125
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.y) dp2add r0.xyw, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (17) (!p0.x) dp2add r0.xyzw, t0, r5, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) dp2add r0.xyzw, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (18) (p0.w) dp2add r0.xz, r4, c5, t2.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.791015625, 0.0, 1.791015625, 0.0
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp2add r0.xz, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (19) (p0.y) dp2add r0.yw, c4, t1, c4.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp2add r0.yw, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (20) dp2add r0.xyw, t0, t0, c6.x";
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

TESTCASE "dp2add - Pred/Mask (21) dp2add r0.xyzw, v0, c5, v0.x";
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

TESTCASE "dp2add - Pred/Mask (22) (p0.y) dp2add r0, t0, r5, t0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp2add r0, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (23) (p0) dp2add r0.x, c4, v1, v1.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) dp2add r0.x, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (24) dp2add r0.y, t0, t0, t0.x";
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

TESTCASE "dp2add - Pred/Mask (25) dp2add r0.z, v0, v0, v0.x";
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

TESTCASE "dp2add - Pred/Mask (26) (p0.y) dp2add r0.w, v0, c5, r6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -3.3330078125
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) dp2add r0.w, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (27) (p0) dp2add r0, r4, v1, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -3.423828125, -3.423828125, -3.423828125, -3.423828125
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0) dp2add r0, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (28) (!p0.z) dp2add r0.x, r4, r5, r6.x";
	USEPREDICATE
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
	(!p0.z) dp2add r0.x, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (29) (!p0.w) dp2add r0.y, c4, c4, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) dp2add r0.y, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (30) (p0) dp2add r0.z, t0, r5, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.791015625, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0) dp2add r0.z, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (31) (p0) dp2add r0.w, c4, t1, c4.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) dp2add r0.w, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (32) (!p0.y) dp2add r0.xz, v0, c5, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -0.7236328125, 0.0, -0.7236328125, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.y) dp2add r0.xz, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (33) (!p0.w) dp2add r0.yw, t0, t0, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 3.189453125, 0.0, 3.189453125
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp2add r0.yw, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (34) (!p0.x) dp2add r0.xyw, r4, c5, t2.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 0.0, 1.791015625
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) dp2add r0.xyw, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (35) (p0.z) dp2add r0.xyzw, c4, v1, v1.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -0.908203125, -0.908203125, -0.908203125, -0.908203125
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) dp2add r0.xyzw, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (36) (p0.w) dp2add r0, v0, v0, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 2.18798828125, 2.18798828125, 2.18798828125, 2.18798828125
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) dp2add r0, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (37) (p0) dp2add r0.y, t0, r5, t0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 1.447265625, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0) dp2add r0.y, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (38) (p0.y) dp2add r0.yw, t0, t0, t0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 2.845703125, 0.0, 2.845703125
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp2add r0.yw, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (39) (p0.w) dp2add r0.z, r4, r5, r6.x";
	USEPREDICATE
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
	(p0.w) dp2add r0.z, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (40) (!p0.x) dp2add r0.x, r4, v1, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) dp2add r0.x, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (41) dp2add r0.x, c4, c4, v0.x";
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

TESTCASE "dp2add - Pred/Mask (42) (p0.y) dp2add r0.x, t0, r5, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.791015625, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp2add r0.x, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (43) dp2add r0.z, v0, c5, r6.x";
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

TESTCASE "dp2add - Pred/Mask (44) (!p0.z) dp2add r0.z, c4, t1, c4.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.447265625, 0.0
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp2add r0.z, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (45) dp2add r0.z, r4, c5, t2.x";
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

TESTCASE "dp2add - Pred/Mask (46) (!p0.x) dp2add r0.z, v0, c5, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, -0.7236328125, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) dp2add r0.z, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (47) (!p0.z) dp2add r0.w, t0, t0, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 3.189453125
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp2add r0.w, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (48) dp2add r0.xz, c4, v1, v1.x";
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

TESTCASE "dp2add - Pred/Mask (49) (p0.z) dp2add r0.xz, t0, r5, t0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp2add r0.xz, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (50) (p0.x) dp2add r0.yw, v0, v0, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 2.18798828125, 0.0, 2.18798828125
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) dp2add r0.yw, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (51) (!p0.w) dp2add r0.xyw, t0, t0, t0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 2.845703125, 2.845703125, 0.0, 2.845703125
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp2add r0.xyw, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (52) (!p0.y) dp2add r0.xyzw, r4, r5, r6.x";
	USEPREDICATE
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
	(!p0.y) dp2add r0.xyzw, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (53) (p0.w) dp2add r0.xyzw, v0, c5, r6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -3.3330078125, -3.3330078125, -3.3330078125, -3.3330078125
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) dp2add r0.xyzw, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (54) (!p0.x) dp2add r0, c4, c4, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 5.798828125, 5.798828125, 5.798828125, 5.798828125
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) dp2add r0, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (55) dp2add r0, t0, r5, c6.x";
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

TESTCASE "dp2add - Pred/Mask (56) (!p0.w) dp2add r0.y, c4, t1, c4.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp2add r0.y, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (57) (!p0.z) dp2add r0.y, r4, c5, t2.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 1.791015625, 0.0, 0.0
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp2add r0.y, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (58) (p0.w) dp2add r0.y, v0, c5, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, -0.7236328125, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) dp2add r0.y, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (59) (p0.x) dp2add r0.y, t0, t0, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp2add r0.y, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (60) (p0.w) dp2add r0.w, r4, v1, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) dp2add r0.w, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (61) (!p0.w) dp2add r0.w, c4, v1, v1.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) dp2add r0.w, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (62) dp2add r0.w, t0, r5, t0.x";
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

TESTCASE "dp2add - Pred/Mask (63) (p0) dp2add r0.xyw, v0, v0, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 2.18798828125, 2.18798828125, 0.0, 2.18798828125
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) dp2add r0.xyw, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (64) (!p0.z) dp2add r0.xyzw, t0, t0, t0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 2.845703125, 2.845703125, 2.845703125, 2.845703125
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp2add r0.xyzw, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (65) (p0) dp2add r0.yw, t0, r5, t0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0) dp2add r0.yw, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (66) (!p0.y) dp2add r0.x, v0, v0, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 2.18798828125, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.y) dp2add r0.x, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (67) (p0.w) dp2add r0.yw, r4, r5, r6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 1.791015625, 0.0, 1.791015625
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp2add r0.yw, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (68) (p0) dp2add r0, v0, c5, r6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -3.3330078125, -3.3330078125, -3.3330078125, -3.3330078125
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(p0) dp2add r0, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (69) (!p0.y) dp2add r0, c4, t1, c4.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.447265625, 1.447265625, 1.447265625, 1.447265625
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) dp2add r0, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (70) (!p0.w) dp2add r0, r4, c5, t2.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.791015625, 1.791015625, 1.791015625, 1.791015625
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp2add r0, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (71) dp2add r0, v0, c5, v0.x";
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

TESTCASE "dp2add - Pred/Mask (72) (!p0.y) dp2add r0, t0, t0, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) dp2add r0, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (73) (p0.x) dp2add r0, t0, t0, t0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp2add r0, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (74) (p0) dp2add r0.x, c4, t1, c4.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) dp2add r0.x, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (75) (p0.w) dp2add r0.x, r4, c5, t2.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 1.791015625, 0.0, 0.0, 0.0
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp2add r0.x, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (76) (p0.w) dp2add r0.x, v0, c5, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -0.7236328125, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) dp2add r0.x, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (77) dp2add r0.x, t0, t0, c6.x";
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

TESTCASE "dp2add - Pred/Mask (78) (!p0.y) dp2add r0.x, t0, t0, t0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 2.845703125, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) dp2add r0.x, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (79) (!p0.z) dp2add r0.y, r4, r5, r6.x";
	USEPREDICATE
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
	(!p0.z) dp2add r0.y, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (80) (!p0.w) dp2add r0.y, v0, c5, r6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, -3.3330078125, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) dp2add r0.y, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (81) (p0.x) dp2add r0.y, t0, r5, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 1.791015625, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp2add r0.y, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (82) (p0.z) dp2add r0.y, c4, v1, v1.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) dp2add r0.y, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (83) (p0.y) dp2add r0.z, r4, v1, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, -3.423828125, 0.0
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) dp2add r0.z, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (84) (!p0.z) dp2add r0.z, t0, t0, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 3.189453125, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp2add r0.z, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (85) dp2add r0.z, c4, v1, v1.x";
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

TESTCASE "dp2add - Pred/Mask (86) (!p0.z) dp2add r0.z, t0, r5, t0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp2add r0.z, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (87) (p0.y) dp2add r0.w, c4, c4, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) dp2add r0.w, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (88) (p0) dp2add r0.w, r4, c5, t2.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.791015625
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0) dp2add r0.w, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (89) dp2add r0.w, v0, c5, v0.x";
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

TESTCASE "dp2add - Pred/Mask (90) (!p0.z) dp2add r0.w, v0, v0, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 2.18798828125
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) dp2add r0.w, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (91) (p0.w) dp2add r0.w, t0, t0, t0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 2.845703125
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp2add r0.w, t0, t0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (92) (p0.y) dp2add r0.xz, r4, r5, r6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 1.791015625, 0.0, 1.791015625, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp2add r0.xz, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (93) (p0.z) dp2add r0.xz, r4, v1, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) dp2add r0.xz, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (94) (p0.x) dp2add r0.xz, c4, c4, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 5.798828125, 0.0, 5.798828125, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) dp2add r0.xz, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (95) (!p0.x) dp2add r0.xz, t0, r5, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.791015625, 0.0, 1.791015625, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) dp2add r0.xz, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (96) (p0.z) dp2add r0.xz, t0, t0, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp2add r0.xz, t0, t0, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (97) (!p0.y) dp2add r0.xz, v0, v0, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 2.18798828125, 0.0, 2.18798828125, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.y) dp2add r0.xz, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (98) dp2add r0.xz, t0, t0, t0.x";
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

TESTCASE "dp2add - Pred/Mask (99) (p0.y) dp2add r0.yw, v0, c5, r6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) dp2add r0.yw, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (100) (p0.y) dp2add r0.yw, c4, c4, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 5.798828125, 0.0, 5.798828125
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) dp2add r0.yw, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (101) (p0) dp2add r0.yw, t0, r5, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0) dp2add r0.yw, t0, r5, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (102) (!p0.z) dp2add r0.yw, v0, c5, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) dp2add r0.yw, v0, c5, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (103) (p0.w) dp2add r0.yw, c4, v1, v1.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, -0.908203125, 0.0, -0.908203125
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) dp2add r0.yw, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (104) (!p0.x) dp2add r0.xyw, r4, r5, r6.x";
	USEPREDICATE
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
	(!p0.x) dp2add r0.xyw, r4, r5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (105) (!p0.w) dp2add r0.xyw, v0, c5, r6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) dp2add r0.xyw, v0, c5, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (106) dp2add r0.xyw, r4, v1, c6.x";
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

TESTCASE "dp2add - Pred/Mask (107) dp2add r0.xyw, t0, r5, c6.x";
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

TESTCASE "dp2add - Pred/Mask (108) (!p0) dp2add r0.xyw, c4, t1, c4.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.447265625, 1.447265625, 0.0, 1.447265625
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp2add r0.xyw, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (109) (p0.x) dp2add r0.xyw, c4, v1, v1.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) dp2add r0.xyw, c4, v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (110) (p0.z) dp2add r0.xyw, t0, r5, t0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp2add r0.xyw, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (111) (!p0.w) dp2add r0.xyzw, r4, v1, c6.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) dp2add r0.xyzw, r4, v1, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (112) (p0.y) dp2add r0.xyzw, c4, c4, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) dp2add r0.xyzw, c4, c4, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (113) (!p0.x) dp2add r0.xyzw, c4, t1, c4.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.447265625, 1.447265625, 1.447265625, 1.447265625
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) dp2add r0.xyzw, c4, t1, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (114) (p0.z) dp2add r0.xyzw, r4, c5, t2.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp2add r0.xyzw, r4, c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (115) (p0.z) dp2add r0.xyzw, t0, r5, t0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp2add r0.xyzw, t0, r5, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dp2add - Pred/Mask (116) (!p0.z) dp2add r0.xyzw, v0, v0, v0.x";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 2.18798828125, 2.18798828125, 2.18798828125, 2.18798828125
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) dp2add r0.xyzw, v0, v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  235 tests generated.