// Tests for Instruction: lrp
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
TESTCASE "lrp - Standard (0) lrp r0, r4, r5, r6";
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

TESTCASE "lrp - Standard (1) lrp_pp r0, -v0.x, c5.x, -r6.x";
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

TESTCASE "lrp - Standard (2) lrp r0, r4.y, -v1.y, -c6.y";
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

TESTCASE "lrp - Standard (3) lrp_pp r0, -c4.zxyw, -c4.zxyw, v0.zxyw";
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

TESTCASE "lrp - Standard (4) lrp r0, -t0.xw, r5.xw, c6.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.9326171875, -5.599609375, -5.599609375, -5.599609375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -t0.xw, r5.xw, c6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (5) lrp_pp r0, c4.wzx, t1.wzx, c4.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.962890625, -3.744140625, -2.337890625, -2.337890625
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, c4.wzx, t1.wzx, c4.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (6) lrp r0, r4.yzwy, c5.yzwy, t2.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.982421875, 2.470703125, 3.349609375, -2.982421875
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, r4.yzwy, c5.yzwy, t2.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (7) lrp_pp r0, -v0, -c5.x, v0.y";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.792236328125, -0.143798828125, -0.60986328125, -0.69091796875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -v0, -c5.x, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (8) lrp r0, t0.x, -t0, -c6.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, -5.7314453125, 2.9169921875, 1.0380859375, 6.908203125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0.x, -t0, -c6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (9) lrp_pp r0, -c4.y, v1.zxyw, -v1";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.19921875, 0.861328125, 0.0380859375, 0.6298828125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4.y, v1.zxyw, -v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (10) lrp r0, t0.zxyw, r5.y, t0.x";
	PASSBEGIN
	TESTBEGIN
	def c1, -5.44921875, -6.029296875, -3.966796875, 1.76953125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0.zxyw, r5.y, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (11) lrp_pp r0, -v0.xw, -v0.wzx, -v0.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.069091796875, -0.84375, -0.83544921875, -0.03125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -v0.xw, -v0.wzx, -v0.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (12) lrp_pp r0, t0.wzx, -t0.xw, -t0.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.96875, -1.8125, -1.8125, -1.8125
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, t0.wzx, -t0.xw, -t0.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (13) lrp_pp r0, -r4.yzwy, -r5.yzwy, -r6.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.0556640625, 1.220703125, 7.005859375, -0.0400390625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4.yzwy, -r5.yzwy, -r6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (14) lrp r0, v0.wzx, -c5, r6.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.4765625, -0.30859375, 0.4384765625, -1.345703125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, v0.wzx, -c5, r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (15) lrp_pp r0, -r4, v1.zxyw, c6.x";
	PASSBEGIN
	TESTBEGIN
	def c1, 2.03564453125, 0.81201171875, 1.1962890625, -5.7880859375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4, v1.zxyw, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (16) lrp r0, c4.x, c4.y, -v0";
	PASSBEGIN
	TESTBEGIN
	def c1, -1.01513671875, 0.46923828125, -0.59765625, -0.783203125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, c4.x, c4.y, -v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (17) lrp_pp r0, t0.y, -r5.x, -c6.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, -2.9560546875, 1.4736328125, 1.166015625, 0.4892578125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, t0.y, -r5.x, -c6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (18) lrp r0, -c4.zxyw, -t1, -c4";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.65234375, -1.123046875, 0.5068359375, -2.662109375
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -c4.zxyw, -t1, -c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (19) lrp_pp r0, -r4.xw, -c5.y, -t2.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.4208984375, 4.740234375, 4.30078125, 3.333984375
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4.xw, -c5.y, -t2.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (20) lrp r0, v0.yzwy, c5.zxyw, -v0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.734619140625, -1.65673828125, -0.00732421875, 0.143310546875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, v0.yzwy, c5.zxyw, -v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (21) lrp_pp r0, -t0, t0.xw, c6.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.7861328125, 1.775390625, 4.17578125, -7.85546875
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -t0, t0.xw, c6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (22) lrp r0, c4.x, -v1.wzx, v1.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.4619140625, 0.6005859375, 1.892578125, 1.892578125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, c4.x, -v1.wzx, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (23) lrp_pp r0, -t0.y, -r5, -t0.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.6689453125, -0.0380859375, 0.515625, -1.240234375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -t0.y, -r5, -t0.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (24) lrp r0, -t0.xw, t0.x, t0.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -5.6318359375, -1.177734375, -1.96875, -1.96875
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -t0.xw, t0.x, t0.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (25) lrp r0, -v0.wzx, -v0.yzwy, v0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.3154296875, 2.52685546875, 2.84545898438, 2.43017578125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, -v0.wzx, -v0.yzwy, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (26) lrp r0, v0.yzwy, v0.xw, -v0.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.22705078125, 0.68896484375, 0.77587890625, 0.189208984375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, v0.yzwy, v0.xw, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (27) lrp r0, -r4.zxyw, r5.xw, r6.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.435546875, 4.2197265625, 1.255859375, -5.599609375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -r4.zxyw, r5.xw, r6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (28) lrp_pp r0, -v0, c5.wzx, -r6.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.56298828125, 2.63916015625, 4.2109375, 3.7587890625
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -v0, c5.wzx, -r6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (29) lrp_pp r0, -c4.y, c4.yzwy, v0.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.90771484375, -1.6064453125, 1.7841796875, -0.91015625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4.y, c4.yzwy, v0.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (30) lrp_pp r0, -r4.zxyw, -v1.x, -c6.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.6201171875, -2.84326171875, -0.82763671875, 4.7783203125
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4.zxyw, -v1.x, -c6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (31) lrp_pp r0, c4.xw, -t1.zxyw, c4.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.798828125, 4.015625, 0.6171875, -1.6484375
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, c4.xw, -t1.zxyw, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (32) lrp r0, t0.yzwy, -r5, c6";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -4.9248046875, -3.7890625, 0.341796875, -0.9130859375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0.yzwy, -r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (33) lrp_pp r0, -r4.x, c5.zxyw, t2.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.6513671875, -2.4169921875, 1.2744140625, 3.7353515625
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4.x, c5.zxyw, t2.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (34) lrp r0, v0.wzx, -c5.y, -v0.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.13037109375, -0.2109375, -0.1064453125, -0.099853515625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, v0.wzx, -c5.y, -v0.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (35) lrp_pp r0, t0.zxyw, t0.wzx, c6";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -7.42578125, -1.0380859375, 3.1376953125, -2.654296875
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, t0.zxyw, t0.wzx, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (36) lrp r0, c4.yzwy, v1, -v1.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.6162109375, -1.884765625, 0.970703125, -1.9736328125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, c4.yzwy, v1, -v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (37) lrp_pp r0, -t0, -r5.yzwy, -t0";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.091796875, 0.484375, -3.427734375, -4.927734375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -t0, -r5.yzwy, -t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (38) lrp r0, t0.x, -t0.yzwy, t0.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -4.783203125, -6.1982421875, 0.6923828125, -4.783203125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0.x, -t0.yzwy, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (39) lrp_pp r0, -r4.y, -r5.wzx, -r6.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.2509765625, 0.5048828125, 1.7763671875, 1.7763671875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4.y, -r5.wzx, -r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (40) lrp_pp r0, v0.zxyw, -c5.xw, -r6.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.732421875, -1.33251953125, -0.07080078125, -1.080078125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, v0.zxyw, -c5.xw, -r6.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (41) lrp_pp r0, r4.xw, -v1, -c6";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 6.57763671875, -1.2783203125, 0.0830078125, -2.1318359375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, r4.xw, -v1, -c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (42) lrp_pp r0, c4.wzx, -c4.x, v0.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 3.1748046875, -1.0546875, -1.185546875, -2.43798828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, c4.wzx, -c4.x, v0.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (43) lrp_pp r0, -c4.yzwy, -t1.y, c4.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.1435546875, 1.1953125, -2.5546875, -0.0341796875
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4.yzwy, -t1.y, c4.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (44) lrp r0, t0, r5.y, -c6.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 3.1552734375, -1.3212890625, 4.208984375, -1.150390625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0, r5.y, -c6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (45) lrp_pp r0, -v0.x, -c5.xw, -v0";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.70678710938, 0.361572265625, -0.3515625, -0.4755859375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -v0.x, -c5.xw, -v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (46) lrp_pp r0, r4.zxyw, -c5, t2.x";
	PASSBEGIN
	TESTBEGIN
	def c1, -7.373046875, -4.6396484375, -3.6533203125, -1.115234375
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, r4.zxyw, -c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (47) lrp r0, c4.zxyw, -v1.yzwy, -v1.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.55078125, -2.1826171875, 0.55859375, 0.4609375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, c4.zxyw, -v1.yzwy, -v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (48) lrp_pp r0, t0.wzx, -t0.zxyw, -c6.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.865234375, -5.001953125, 1.4326171875, 7.9287109375
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, t0.wzx, -t0.zxyw, -c6.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (49) lrp r0, v0, v0, v0.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.984375, 0.701904296875, 0.853515625, 0.90625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, v0, v0, v0.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (50) lrp r0, -t0.x, r5.x, t0.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.568359375, -1.8720703125, -5.2626953125, -2.568359375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -t0.x, r5.x, t0.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (51) lrp r0, t0.y, -t0, t0.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.814453125, -4.2607421875, 1.93359375, -0.1513671875
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0.y, -t0, t0.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (52) lrp r0, -t0.zxyw, r5.zxyw, c6.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.21875, -2.083984375, 0.044921875, -6.56640625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -t0.zxyw, r5.zxyw, c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (53) lrp_pp r0, r4.xw, -r5.x, -r6.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.853515625, 2.03515625, 2.03515625, 2.03515625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, r4.xw, -r5.x, -r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (54) lrp_pp r0, v0.xw, c5.y, -r6.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.11767578125, 0.1904296875, 0.1904296875, 0.1904296875
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, v0.xw, c5.y, -r6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (55) lrp_pp r0, c4.xw, c4.xw, -v0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.95361328125, 4.0849609375, 4.0849609375, 4.0849609375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, c4.xw, c4.xw, -v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (56) lrp r0, r4.wzx, -v1.wzx, c6.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.7255859375, -3.9716796875, -2.60693359375, -1.58642578125
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, r4.wzx, -v1.wzx, c6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (57) lrp r0, t0.yzwy, -t0.x, c6.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -5.1064453125, -7.689453125, 4.888671875, -5.1064453125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0.yzwy, -t0.x, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (58) lrp_pp r0, c4, -t1.x, -c4.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.630859375, 1.5966796875, -7.876953125, 2.44140625
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, c4, -t1.x, -c4.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (59) lrp_pp r0, -v0.x, v0.x, -v0";
	PASSBEGIN
	TESTBEGIN
	def c1, -2.92236328125, -1.93017578125, -2.64331054688, -2.76733398438
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -v0.x, v0.x, -v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (60) lrp_pp r0, r4.y, -c5.xw, t2";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -4.9248046875, -1.58984375, 2.5322265625, -0.9130859375
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, r4.y, -c5.xw, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (61) lrp_pp r0, -v0.zxyw, c5.wzx, v0.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.68115234375, 2.259765625, 2.2080078125, 3.341796875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -v0.zxyw, c5.wzx, v0.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (62) lrp_pp r0, t0.zxyw, -t0.zxyw, -t0";
	PASSBEGIN
	TESTBEGIN
	def c1, 2.443359375, -1.0, 2.3837890625, -1.8125
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, t0.zxyw, -t0.zxyw, -t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (63) lrp_pp r0, -c4.xw, -v1.x, v1.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.6162109375, 4.119140625, 4.119140625, 4.119140625
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4.xw, -v1.x, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (64) lrp r0, t0.xw, r5.zxyw, t0.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.953125, -2.44140625, 0.95703125, 3.22265625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0.xw, r5.zxyw, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (65) lrp r0, -r4.x, -r5.zxyw, -r6.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.6513671875, 2.4169921875, -1.2744140625, -3.7353515625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -r4.x, -r5.zxyw, -r6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (66) lrp_pp r0, -r4.x, v1.xw, -c6.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.66357421875, 1.92822265625, -0.25146484375, -0.25146484375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4.x, v1.xw, -c6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (67) lrp r0, v0.zxyw, -v0.zxyw, v0.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.5703125, -0.955810546875, 0.27294921875, -0.72900390625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, v0.zxyw, -v0.zxyw, v0.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (68) lrp r0, -v0.xw, -c5.yzwy, -v0.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.58203125, -2.30126953125, 0.29443359375, -1.642578125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, -v0.xw, -c5.yzwy, -v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (69) lrp r0, -t0.wzx, -r5.wzx, -c6.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 7.005859375, -0.326171875, 1.9326171875, 1.9326171875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -t0.wzx, -r5.wzx, -c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (70) lrp r0, -r4.wzx, -c5.x, -t2.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.90234375, 1.99609375, 2.083984375, 2.083984375
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -r4.wzx, -c5.x, -t2.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (71) lrp_pp r0, -c4.wzx, v1.y, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.974609375, 0.240234375, 0.955078125, 0.955078125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4.wzx, v1.y, -v1.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (72) lrp_pp r0, t0.wzx, -r5.xw, -t0.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.857421875, 7.55859375, 5.521484375, -2.7353515625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, t0.wzx, -r5.xw, -t0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (73) lrp_pp r0, -v0.yzwy, -c5.zxyw, -r6";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.18505859375, 1.205078125, -1.1064453125, 2.32080078125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -v0.yzwy, -c5.zxyw, -r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (74) lrp r0, -c4.yzwy, -c4.wzx, v0.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.74072265625, 2.5146484375, -2.2060546875, 1.92236328125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, -c4.yzwy, -c4.wzx, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (75) lrp_pp r0, -t0, -t0.y, t0.x";
	PASSBEGIN
	TESTBEGIN
	def c1, 3.814453125, 0.876953125, 2.98828125, -7.29296875
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -t0, -t0.y, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (76) lrp_pp r0, -c4.x, t1.xw, -c4.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -5.4140625, 0.73828125, 0.73828125, 0.73828125
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4.x, t1.xw, -c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (77) lrp r0, t0.y, t0.y, c6.y";
	PASSBEGIN
	TESTBEGIN
	def c1, -1.953125, -1.953125, -1.953125, -1.953125
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0.y, t0.y, c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (78) lrp r0, -r4.wzx, r5.y, r6.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -4.30078125, -0.271484375, 1.2744140625, 1.607421875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -r4.wzx, r5.y, r6.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (79) lrp_pp r0, -c4, c4, -v0.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.998046875, -0.96484375, -2.1708984375, -6.0537109375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4, c4, -v0.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (80) lrp r0, -r4, -c5.wzx, -t2.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.7353515625, 0.4345703125, 1.888671875, 1.341796875
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -r4, -c5.wzx, -t2.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (81) lrp r0, -c4, -v1.xw, v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.798828125, -0.6435546875, -1.595703125, 1.349609375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, -c4, -v1.xw, v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (82) lrp r0, t0.x, r5.yzwy, -c6.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 4.17578125, -0.9326171875, 0.6943359375, 4.17578125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0.x, r5.yzwy, -c6.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (83) lrp r0, v0.y, -c5.yzwy, -r6.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.53466796875, -0.09521484375, 0.18701171875, 0.79248046875
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, v0.y, -c5.yzwy, -r6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (84) lrp_pp r0, -c4.y, t1.yzwy, c4.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.029296875, -0.3974609375, 1.3583984375, 0.1474609375
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4.y, t1.yzwy, c4.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (85) lrp r0, -v0.y, c5, v0.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.2080078125, 1.20703125, 1.68823242188, 0.810302734375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, -v0.y, c5, v0.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (86) lrp r0, v0.y, -v0.y, -v0.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.669677734375, -0.7421875, -0.484375, -0.701904296875
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, v0.y, -v0.y, -v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (87) lrp r0, t0.xw, t0.yzwy, -c6.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 6.7314453125, -3.97265625, 2.37109375, -2.669921875
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0.xw, t0.yzwy, -c6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (88) lrp r0, r4.yzwy, -v1.yzwy, c6.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.84619140625, 2.0751953125, -0.3037109375, -2.84619140625
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp r0, r4.yzwy, -v1.yzwy, c6.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (89) lrp r0, t0.yzwy, -r5.wzx, -t0.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 5.177734375, -5.662109375, 4.701171875, -5.2939453125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, t0.yzwy, -r5.wzx, -t0.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (90) lrp r0, -t0.yzwy, t0.wzx, t0.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.703125, -1.494140625, 0.84375, -1.8505859375
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -t0.yzwy, t0.wzx, t0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (91) lrp r0, c4.wzx, -t1.xw, c4";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 4.828125, -0.3359375, -2.3642578125, 8.0263671875
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, c4.wzx, -t1.xw, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * sat * pp
TESTCASE "lrp - sat (0) lrp r0, r4, r5, r6";
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

TESTCASE "lrp - sat (1) lrp_pp r0, v0, c5, r6";
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

TESTCASE "lrp - sat (2) lrp_sat r0, r4, v1, c6";
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

TESTCASE "lrp - sat (3) lrp_sat_pp r0, c4, c4, v0";
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

TESTCASE "lrp - sat (4) lrp r0, t0, r5, c6";
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

TESTCASE "lrp - sat (5) lrp r0, c4, t1, c4";
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

TESTCASE "lrp - sat (6) lrp r0, r4, c5, t2";
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

TESTCASE "lrp - sat (7) lrp r0, v0, c5, v0";
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

TESTCASE "lrp - sat (8) lrp r0, t0, t0, c6";
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

TESTCASE "lrp - sat (9) lrp r0, c4, v1, v1";
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

TESTCASE "lrp - sat (10) lrp r0, t0, r5, t0";
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

TESTCASE "lrp - sat (11) lrp r0, v0, v0, v0";
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

TESTCASE "lrp - sat (12) lrp r0, t0, t0, t0";
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

TESTCASE "lrp - sat (13) lrp_sat r0, v0, c5, r6";
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

TESTCASE "lrp - sat (14) lrp r0, c4, c4, v0";
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

TESTCASE "lrp - sat (15) lrp_sat_pp r0, r4, r5, r6";
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

TESTCASE "lrp - sat (16) lrp_pp r0, r4, v1, c6";
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

TESTCASE "lrp - sat (17) lrp_sat_pp r0, t0, r5, c6";
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

TESTCASE "lrp - sat (18) lrp_sat_pp r0, c4, t1, c4";
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

TESTCASE "lrp - sat (19) lrp_sat_pp r0, r4, c5, t2";
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

TESTCASE "lrp - sat (20) lrp_sat_pp r0, v0, c5, v0";
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

TESTCASE "lrp - sat (21) lrp_sat_pp r0, t0, t0, c6";
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

TESTCASE "lrp - sat (22) lrp_sat_pp r0, c4, v1, v1";
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

TESTCASE "lrp - sat (23) lrp_sat_pp r0, t0, r5, t0";
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

TESTCASE "lrp - sat (24) lrp_sat_pp r0, v0, v0, v0";
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

TESTCASE "lrp - sat (25) lrp_sat_pp r0, t0, t0, t0";
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
TESTCASE "lrp - Pred/Mask (0) lrp r0, r4, r5, r6";
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

TESTCASE "lrp - Pred/Mask (1) (!p0.x) lrp_pp r0.x, v0, c5, r6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) lrp_pp r0.x, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (2) (!p0.y) lrp_sat r0.y, r4, v1, c6";
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
	(!p0.y) lrp_sat r0.y, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (3) (p0.z) lrp_sat_pp r0.z, c4, c4, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) lrp_sat_pp r0.z, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (4) (p0.w) lrp r0.w, t0, r5, c6";
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
	(p0.w) lrp r0.w, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (5) (p0) lrp_sat r0.xz, c4, t1, c4";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) lrp_sat r0.xz, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (6) lrp_sat r0.yw, r4, c5, t2";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.yw, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (7) (p0.y) lrp r0.xyw, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -1.73803710938, 0.295166015625, 0.0, 1.302734375
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) lrp r0.xyw, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (8) (p0.z) lrp r0.xyzw, t0, t0, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) lrp r0.xyzw, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (9) (!p0.w) lrp_sat_pp r0, c4, v1, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.890625, 0.046875, 0.234375, 0.671875
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) lrp_sat_pp r0, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (10) lrp_sat_pp r0.x, t0, r5, t0";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.x, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (11) lrp_pp r0.y, v0, v0, v0";
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

TESTCASE "lrp - Pred/Mask (12) lrp r0.z, t0, t0, t0";
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

TESTCASE "lrp - Pred/Mask (13) (!p0.x) lrp_sat_pp r0.w, r4, r5, r6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) lrp_sat_pp r0.w, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (14) (p0.w) lrp_sat r0.xz, v0, c5, r6";
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
	(p0.w) lrp_sat r0.xz, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (15) (p0) lrp_pp r0.yw, r4, v1, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, -2.93701171875, 0.0, 2.1318359375
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0) lrp_pp r0.yw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (16) lrp r0.xyw, c4, c4, v0";
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

TESTCASE "lrp - Pred/Mask (17) (!p0.y) lrp_sat_pp r0.xyzw, t0, r5, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(!p0.y) lrp_sat_pp r0.xyzw, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (18) (!p0.z) lrp_pp r0.xz, r4, c5, t2";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -1.3173828125, 0.0, 2.470703125, 0.0
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) lrp_pp r0.xz, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (19) (!p0.w) lrp_pp r0.x, c4, t1, c4";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) lrp_pp r0.x, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (20) (!p0) lrp_sat_pp r0.xyw, t0, t0, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) lrp_sat_pp r0.xyw, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (21) (!p0.x) lrp_sat_pp r0.yw, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.295166015625, 0.0, 1.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) lrp_sat_pp r0.yw, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (22) (p0.x) lrp r0, t0, r5, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -2.337890625, -1.998046875, -3.744140625, 0.962890625
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) lrp r0, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (23) (p0.y) lrp r0.x, c4, v1, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.890625, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) lrp r0.x, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (24) (p0) lrp_sat_pp r0.y, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) lrp_sat_pp r0.y, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (25) (!p0) lrp_sat r0.z, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.84375, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) lrp_sat r0.z, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (26) lrp_sat r0.w, v0, c5, r6";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.w, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (27) (!p0.w) lrp_sat_pp r0.xyzw, r4, r5, r6";
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
	mov r2, c0.w
	(!p0.w) lrp_sat_pp r0.xyzw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (28) (p0.y) lrp_sat r0.yw, c4, c4, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) lrp_sat r0.yw, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (29) (p0.z) lrp_pp r0, r4, v1, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -6.57763671875, -2.93701171875, 1.2841796875, 2.1318359375
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) lrp_pp r0, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (30) lrp r0.y, t0, r5, c6";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -2.982421875, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.y, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (31) (!p0.x) lrp_pp r0.z, c4, t1, c4";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, -3.744140625, 0.0
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) lrp_pp r0.z, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (32) (p0) lrp r0.w, r4, c5, t2";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0) lrp r0.w, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (33) lrp_sat r0.xz, v0, c5, v0";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.xz, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (34) (p0.x) lrp_pp r0.xyw, c4, v1, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.890625, 0.046875, 0.0, 0.671875
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) lrp_pp r0.xyw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (35) (p0.y) lrp_sat_pp r0.xyzw, t0, r5, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) lrp_sat_pp r0.xyzw, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (36) (p0.w) lrp r0.y, t0, t0, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) lrp r0.y, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (37) (p0.x) lrp r0.xz, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.984375, 0.0, 0.84375, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) lrp r0.xz, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (38) (p0.z) lrp_sat r0.yw, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) lrp_sat r0.yw, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (39) (!p0) lrp_sat r0.xyzw, v0, c5, r6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0) lrp_sat r0.xyzw, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (40) (p0) lrp_pp r0, c4, c4, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 1.89208984375, 0.0, 2.548828125
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) lrp_pp r0, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (41) (!p0.z) lrp_pp r0.x, r4, r5, r6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -1.3173828125, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) lrp_pp r0.x, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (42) lrp_sat r0.z, r4, v1, c6";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.z, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (43) (p0.z) lrp_pp r0.w, c4, t1, c4";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.962890625
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) lrp_pp r0.w, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (44) (p0) lrp r0.xz, t0, r5, c6";
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
	(p0) lrp r0.xz, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (45) (!p0.y) lrp_pp r0.xyw, r4, c5, t2";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(!p0.y) lrp_pp r0.xyw, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (46) (!p0) lrp_pp r0.xyzw, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.295166015625, 0.0, 1.302734375
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) lrp_pp r0.xyzw, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (47) lrp_pp r0.xyzw, c4, v1, v1";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.890625, 0.046875, 0.234375, 0.671875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.xyzw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (48) lrp_sat r0, t0, t0, c6";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (49) (p0.z) lrp_sat_pp r0.y, t0, r5, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) lrp_sat_pp r0.y, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (50) (!p0.x) lrp_sat_pp r0.xz, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) lrp_sat_pp r0.xz, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (51) (p0.w) lrp r0.yw, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.484375, 0.0, 0.90625
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) lrp r0.yw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (52) (p0.y) lrp_pp r0.y, r4, r5, r6";
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
	mov r2, c0.w
	(p0.y) lrp_pp r0.y, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (53) (p0.z) lrp_pp r0.y, v0, c5, r6";
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
	(p0.z) lrp_pp r0.y, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (54) (!p0.z) lrp_sat_pp r0.z, t0, r5, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) lrp_sat_pp r0.z, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (55) (!p0.w) lrp r0.z, r4, c5, t2";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) lrp r0.z, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (56) (p0.z) lrp_sat r0.z, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) lrp_sat r0.z, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (57) (!p0.x) lrp_sat_pp r0.z, t0, t0, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) lrp_sat_pp r0.z, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (58) (p0.z) lrp_sat_pp r0.z, c4, v1, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) lrp_sat_pp r0.z, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (59) (!p0.w) lrp_sat_pp r0.z, t0, r5, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) lrp_sat_pp r0.z, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (60) (!p0.x) lrp_pp r0.xz, r4, v1, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -6.57763671875, 0.0, 1.2841796875, 0.0
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) lrp_pp r0.xz, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (61) (p0.x) lrp r0.xz, c4, c4, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) lrp r0.xz, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (62) lrp_sat r0.xyw, c4, t1, c4";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.962890625
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.xyw, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (63) (!p0.z) lrp_sat r0.xyw, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.984375, 0.484375, 0.0, 0.90625
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) lrp_sat r0.xyw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (64) (p0.x) lrp r0.xyzw, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) lrp r0.xyzw, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (65) (!p0.y) lrp_sat r0, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.984375, 0.484375, 0.84375, 0.90625
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.y) lrp_sat r0, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (66) (!p0.w) lrp_sat_pp r0, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) lrp_sat_pp r0, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (67) (p0.z) lrp_pp r0.z, r4, r5, r6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 2.470703125, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) lrp_pp r0.z, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (68) (!p0.y) lrp r0.xz, r4, r5, r6";
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
	(!p0.y) lrp r0.xz, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (69) (p0.y) lrp_sat_pp r0, v0, c5, r6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) lrp_sat_pp r0, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (70) (!p0) lrp_sat_pp r0, t0, r5, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(!p0) lrp_sat_pp r0, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (71) (!p0.w) lrp_pp r0, c4, t1, c4";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -2.337890625, -1.998046875, -3.744140625, 0.962890625
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) lrp_pp r0, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (72) lrp_sat r0, r4, c5, t2";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (73) (!p0.y) lrp_sat r0, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.y) lrp_sat r0, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (74) (!p0.x) lrp_pp r0.x, r4, v1, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, -6.57763671875, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) lrp_pp r0.x, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (75) (p0.z) lrp_pp r0.x, c4, c4, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 6.79833984375, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) lrp_pp r0.x, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (76) (!p0.w) lrp r0.x, t0, r5, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -1.3173828125, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) lrp r0.x, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (77) (p0) lrp_sat_pp r0.x, r4, c5, t2";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0) lrp_sat_pp r0.x, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (78) (!p0.x) lrp_sat r0.x, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) lrp_sat r0.x, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (79) (p0.z) lrp r0.x, t0, t0, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) lrp r0.x, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (80) (!p0) lrp_sat r0.x, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) lrp_sat r0.x, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (81) (!p0.w) lrp_pp r0.x, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) lrp_pp r0.x, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (82) (!p0.z) lrp_pp r0.y, c4, c4, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 1.89208984375, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) lrp_pp r0.y, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (83) lrp_sat r0.y, c4, t1, c4";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.y, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (84) (!p0) lrp_pp r0.y, r4, c5, t2";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(!p0) lrp_pp r0.y, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (85) (p0.y) lrp r0.y, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.295166015625, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) lrp r0.y, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (86) (p0.z) lrp_pp r0.y, c4, v1, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) lrp_pp r0.y, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (87) (p0.x) lrp_sat_pp r0.z, v0, c5, r6";
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
	(p0.x) lrp_sat_pp r0.z, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (88) (p0.w) lrp_sat r0.w, r4, v1, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) lrp_sat r0.w, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (89) lrp_sat_pp r0.w, c4, c4, v0";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.w, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (90) (!p0) lrp_sat_pp r0.w, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) lrp_sat_pp r0.w, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (91) (!p0) lrp_sat r0.w, t0, t0, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) lrp_sat r0.w, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (92) lrp_sat r0.w, c4, v1, v1";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.671875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat r0.w, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (93) (p0.y) lrp_pp r0.w, t0, r5, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.962890625
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) lrp_pp r0.w, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (94) (!p0.w) lrp_sat r0.w, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.90625
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) lrp_sat r0.w, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (95) (!p0.z) lrp_sat r0.w, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) lrp_sat r0.w, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (96) (!p0.x) lrp_sat r0.xz, t0, t0, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) lrp_sat r0.xz, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (97) (p0) lrp_sat_pp r0.xz, c4, v1, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) lrp_sat_pp r0.xz, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (98) lrp_sat_pp r0.xz, t0, r5, t0";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.xz, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (99) (!p0.z) lrp r0.yw, r4, r5, r6";
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
	(!p0.z) lrp r0.yw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (100) (!p0) lrp r0.yw, v0, c5, r6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0) lrp r0.yw, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (101) (p0.z) lrp_sat r0.yw, t0, r5, c6";
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
	(p0.z) lrp_sat r0.yw, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (102) (!p0) lrp_pp r0.yw, c4, t1, c4";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, -1.998046875, 0.0, 0.962890625
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) lrp_pp r0.yw, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (103) lrp r0.yw, t0, t0, c6";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.953125, 0.0, 4.19921875
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0.yw, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (104) (!p0.y) lrp_sat r0.yw, c4, v1, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.046875, 0.0, 0.671875
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.y) lrp_sat r0.yw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (105) (!p0) lrp_pp r0.yw, t0, r5, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, -1.998046875, 0.0, 0.962890625
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(!p0) lrp_pp r0.yw, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (106) (!p0.z) lrp_sat_pp r0.xyw, r4, r5, r6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) lrp_sat_pp r0.xyw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (107) (p0.x) lrp_pp r0.xyw, v0, c5, r6";
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
	(p0.x) lrp_pp r0.xyw, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (108) (p0.x) lrp_pp r0.xyw, r4, v1, c6";
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
	(p0.x) lrp_pp r0.xyw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (109) (!p0) lrp_sat_pp r0.xyw, t0, r5, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(!p0) lrp_sat_pp r0.xyw, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (110) lrp_sat_pp r0.xyw, t0, r5, t0";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.962890625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.xyw, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (111) (!p0.w) lrp_sat_pp r0.xyw, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) lrp_sat_pp r0.xyw, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (112) (p0.w) lrp r0.xyzw, r4, v1, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -6.57763671875, -2.93701171875, 1.2841796875, 2.1318359375
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) lrp r0.xyzw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (113) (p0.y) lrp r0.xyzw, c4, c4, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) lrp r0.xyzw, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (114) (!p0.x) lrp_pp r0.xyzw, c4, t1, c4";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) lrp_pp r0.xyzw, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (115) (!p0) lrp r0.xyzw, r4, c5, t2";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, -2.982421875, 0.0, 3.349609375
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0) lrp r0.xyzw, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (116) (p0.x) lrp_sat_pp r0.xyzw, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) lrp_sat_pp r0.xyzw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  235 tests generated.