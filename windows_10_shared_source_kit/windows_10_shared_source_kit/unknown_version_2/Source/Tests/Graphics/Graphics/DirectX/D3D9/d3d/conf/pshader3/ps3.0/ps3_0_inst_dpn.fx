// Tests for Instruction: dpN
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
	PixelShaderConstantF[4] = {-0.46875, -1.78125, 1.34375, 1.96875}; \
	PixelShaderConstantF[5] = {0.6875, -0.25, -1.0, 1.21875};

VertexShader NormalVS =
	asm
	{
		vs_3_0
		dcl_position	v0
		dcl_position	o0
		dcl_texcoord0 o1
		dcl_texcoord1 o2
		def c5, -0.46875, -1.78125, 1.34375, 1.96875
		def c6, 0.6875, -0.25, -1.0, 1.21875
		m4x4 o0, v0, c0
		mov o1, c5
		mov o2, c6
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

TESTCASE "dpN - Standard (1) dp3_pp r0.xyz, -c4_abs.x, -v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.966796875, 0.966796875, 0.966796875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -c4_abs.x, -v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (2) dp3 r0.xyz, -v0_abs.y, c5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3359375, -1.3359375, -1.3359375, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -v0_abs.y, c5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (3) dp3_pp r0.xyz, r4.zxyw, -r5_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.576171875, -0.576171875, -0.576171875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, r4.zxyw, -r5_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (4) dp3 r0.xyz, c4.xw, v1_abs.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.4765625, 4.4765625, 4.4765625, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, c4.xw, v1_abs.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (5) dp3_pp r0.xyz, v0.wzx, c5.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.7333984375, 0.7333984375, 0.7333984375, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, v0.wzx, c5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (6) dp3 r0.xyz, -r4.yzwy, -r5.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.5009765625, 1.5009765625, 1.5009765625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -r4.yzwy, -r5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (7) dp3_pp r0.xyz, -v0_abs, -c5_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.470703125, 2.470703125, 2.470703125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -v0_abs, -c5_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (8) dp3_pp r0.xyz, -r4_abs.x, -r5_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.908203125, 0.908203125, 0.908203125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -r4_abs.x, -r5_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (9) dp3 r0.xyz, -c4_abs.y, v1.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.001953125, 1.001953125, 1.001953125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -c4_abs.y, v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (10) dp3 r0.xyz, -c4_abs.zxyw, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.8984375, 0.8984375, 0.8984375, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -c4_abs.zxyw, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (11) dp3_pp r0.xyz, -r4_abs.xw, -r5.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.0439453125, -0.0439453125, -0.0439453125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -r4_abs.xw, -r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (12) dp3_pp r0.xyz, -r4_abs.wzx, -r5.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.5625, 3.5625, 3.5625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -r4_abs.wzx, -r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (13) dp3_pp r0.xyz, v0.x, c5_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.1572265625, -1.1572265625, -1.1572265625, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, v0.x, c5_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (14) dp3_pp r0.xyz, r4.yzwy, r5_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.052734375, 1.052734375, 1.052734375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, r4.yzwy, r5_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (15) dp3_pp r0.xyz, c4.y, -v1_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.1767578125, 5.1767578125, 5.1767578125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, c4.y, -v1_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (16) dp3_pp r0.xyz, c4.wzx, -v1_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.7109375, -0.7109375, -0.7109375, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, c4.wzx, -v1_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (17) dp3 r0.xyz, c4_abs.yzwy, -v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.080078125, 1.080078125, 1.080078125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, c4_abs.yzwy, -v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (18) dp3 r0.xyz, -c4_abs, -v1_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.5361328125, 3.5361328125, 3.5361328125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -c4_abs, -v1_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (19) dp3 r0.xyz, r4_abs.x, -r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.3515625, 0.3515625, 0.3515625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, r4_abs.x, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (20) dp3_pp r0.xyz, -r4_abs.y, -r5_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.451171875, 3.451171875, 3.451171875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -r4_abs.y, -r5_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (21) dp3 r0.xyz, -v0.zxyw, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.822265625, -2.822265625, -2.822265625, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -v0.zxyw, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (22) dp3 r0.xyz, -v0.xw, -c5_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.384765625, 2.384765625, 2.384765625, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -v0.xw, -c5_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (23) dp3 r0.xyz, v0_abs.wzx, c5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.162109375, -1.162109375, -1.162109375, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, v0_abs.wzx, c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (24) dp3_pp r0.xyz, -v0.yzwy, c5.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.8125, -2.8125, -2.8125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -v0.yzwy, c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (25) dp3 r0.xyz, r4, -r5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.4287109375, 1.4287109375, 1.4287109375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, r4, -r5_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (26) dp3_pp r0.xyz, -r4, -r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.2265625, 0.2265625, 0.2265625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -r4, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (27) dp3_pp r0.xyz, -v0_abs, -c5_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.029296875, 2.029296875, 2.029296875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -v0_abs, -c5_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (28) dp3_pp r0.xyz, v0_abs, -c5.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.130859375, -4.130859375, -4.130859375, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, v0_abs, -c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (29) dp3_pp r0.xyz, -c4_abs.x, v1_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.908203125, -0.908203125, -0.908203125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -c4_abs.x, v1_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (30) dp3 r0.xyz, -c4_abs.x, -v1.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.46484375, 1.46484375, 1.46484375, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -c4_abs.x, -v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (31) dp3 r0.xyz, -v0_abs.x, -c5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.3623046875, 1.3623046875, 1.3623046875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -v0_abs.x, -c5_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (32) dp3 r0.xyz, r4_abs.y, -r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.673828125, -3.673828125, -3.673828125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, r4_abs.y, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (33) dp3_pp r0.xyz, -r4_abs.y, -r5.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.56640625, 5.56640625, 5.56640625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -r4_abs.y, -r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (34) dp3 r0.xyz, c4.y, v1_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.3974609375, -4.3974609375, -4.3974609375, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, c4.y, v1_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (35) dp3 r0.xyz, r4_abs.zxyw, r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.470703125, 2.470703125, 2.470703125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, r4_abs.zxyw, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (36) dp3 r0.xyz, -c4_abs.zxyw, v1.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.666015625, -3.666015625, -3.666015625, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -c4_abs.zxyw, v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (37) dp3_pp r0.xyz, -r4.zxyw, -r5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.0556640625, -0.0556640625, -0.0556640625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -r4.zxyw, -r5_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (38) dp3_pp r0.xyz, c4.zxyw, v1.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.0380859375, -2.0380859375, -2.0380859375, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, c4.zxyw, v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (39) dp3_pp r0.xyz, r4_abs.xw, r5_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.783203125, 2.783203125, 2.783203125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, r4_abs.xw, r5_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (40) dp3_pp r0.xyz, c4_abs.xw, -v1_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.1015625, -1.1015625, -1.1015625, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, c4_abs.xw, -v1_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (41) dp3 r0.xyz, -r4_abs.xw, r5_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.314453125, -2.314453125, -2.314453125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -r4_abs.xw, r5_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (42) dp3 r0.xyz, -v0_abs.xw, c5.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.3134765625, -0.3134765625, -0.3134765625, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -v0_abs.xw, c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (43) dp3_pp r0.xyz, -v0_abs.wzx, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.548828125, -0.548828125, -0.548828125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -v0_abs.wzx, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (44) dp3 r0.xyz, c4_abs.wzx, v1_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.599609375, 2.599609375, 2.599609375, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, c4_abs.wzx, v1_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (45) dp3_pp r0.xyz, -v0.wzx, -c5.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.4072265625, -2.4072265625, -2.4072265625, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -v0.wzx, -c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (46) dp3 r0.xyz, r4_abs.yzwy, -r5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.2734375, -1.2734375, -1.2734375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, r4_abs.yzwy, -r5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (47) dp3_pp r0.xyz, c4_abs.yzwy, -v1.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.349609375, 1.349609375, 1.349609375, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, c4_abs.yzwy, -v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (48) dp3_pp r0.xyz, -v0_abs.yzwy, -c5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.8681640625, 4.8681640625, 4.8681640625, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -v0_abs.yzwy, -c5_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (49) dp4 r0.xyz, r4, r5"
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

TESTCASE "dpN - Standard (50) dp4_pp r0.xyz, -c4_abs.x, -v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.2890625, 1.2890625, 1.2890625, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -c4_abs.x, -v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (51) dp4 r0.xyz, -v0_abs.y, c5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.78125, -1.78125, -1.78125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -v0_abs.y, c5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (52) dp4_pp r0.xyz, r4.zxyw, -r5_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.9755859375, -2.9755859375, -2.9755859375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, r4.zxyw, -r5_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (53) dp4 r0.xyz, c4.xw, v1_abs.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 6.8759765625, 6.8759765625, 6.8759765625, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, c4.xw, v1_abs.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (54) dp4_pp r0.xyz, v0.wzx, c5.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.4111328125, 0.4111328125, 0.4111328125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, v0.wzx, c5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (55) dp4 r0.xyz, -r4.yzwy, -r5.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.9462890625, 1.9462890625, 1.9462890625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -r4.yzwy, -r5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (56) dp4_pp r0.xyz, -v0_abs, -c5_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.82421875, 3.82421875, 3.82421875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -v0_abs, -c5_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (57) dp4_pp r0.xyz, -r4_abs.x, -r5_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.4794921875, 1.4794921875, 1.4794921875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -r4_abs.x, -r5_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (58) dp4 r0.xyz, -c4_abs.y, v1.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.1689453125, -1.1689453125, -1.1689453125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -c4_abs.y, v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (59) dp4 r0.xyz, -c4_abs.zxyw, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.390625, 1.390625, 1.390625, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -c4_abs.zxyw, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (60) dp4_pp r0.xyz, -r4_abs.xw, -r5.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.3095703125, 1.3095703125, 1.3095703125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -r4_abs.xw, -r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (61) dp4_pp r0.xyz, -r4_abs.wzx, -r5.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.1337890625, 4.1337890625, 4.1337890625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -r4_abs.wzx, -r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (62) dp4_pp r0.xyz, v0.x, c5_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.2744140625, -1.2744140625, -1.2744140625, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, v0.x, c5_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (63) dp4_pp r0.xyz, r4.yzwy, r5_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.171875, -0.171875, -0.171875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, r4.yzwy, r5_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (64) dp4_pp r0.xyz, c4.y, -v1_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 6.4013671875, 6.4013671875, 6.4013671875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, c4.y, -v1_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (65) dp4_pp r0.xyz, c4.wzx, -v1_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.59375, -0.59375, -0.59375, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, c4.wzx, -v1_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (66) dp4 r0.xyz, c4_abs.yzwy, -v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.0908203125, -1.0908203125, -1.0908203125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, c4_abs.yzwy, -v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (67) dp4 r0.xyz, -c4_abs, -v1_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.0283203125, 4.0283203125, 4.0283203125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -c4_abs, -v1_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (68) dp4 r0.xyz, r4_abs.x, -r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.46875, 0.46875, 0.46875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, r4_abs.x, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (69) dp4_pp r0.xyz, -r4_abs.y, -r5_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.6220703125, 5.6220703125, 5.6220703125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -r4_abs.y, -r5_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (70) dp4 r0.xyz, -v0.zxyw, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.2216796875, -5.2216796875, -5.2216796875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -v0.zxyw, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (71) dp4 r0.xyz, -v0.xw, -c5_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.73828125, 3.73828125, 3.73828125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -v0.xw, -c5_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (72) dp4 r0.xyz, v0_abs.wzx, c5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.5908203125, -0.5908203125, -0.5908203125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, v0_abs.wzx, c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (73) dp4_pp r0.xyz, -v0.yzwy, c5.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.6416015625, -0.6416015625, -0.6416015625, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -v0.yzwy, c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (74) dp4 r0.xyz, r4, -r5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0751953125, 0.0751953125, 0.0751953125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, r4, -r5_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (75) dp4_pp r0.xyz, -r4, -r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.265625, -0.265625, -0.265625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -r4, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (76) dp4_pp r0.xyz, -v0_abs, -c5_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.4287109375, 4.4287109375, 4.4287109375, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -v0_abs, -c5_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (77) dp4_pp r0.xyz, v0_abs, -c5.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -6.5302734375, -6.5302734375, -6.5302734375, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, v0_abs, -c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (78) dp4_pp r0.xyz, -c4_abs.x, v1_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.4794921875, -1.4794921875, -1.4794921875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -c4_abs.x, v1_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (79) dp4 r0.xyz, -c4_abs.x, -v1.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.0361328125, 2.0361328125, 2.0361328125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -c4_abs.x, -v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (80) dp4 r0.xyz, -v0_abs.x, -c5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.6845703125, 1.6845703125, 1.6845703125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -v0_abs.x, -c5_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (81) dp4 r0.xyz, r4_abs.y, -r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.8984375, -4.8984375, -4.8984375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, r4_abs.y, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (82) dp4_pp r0.xyz, -r4_abs.y, -r5.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 7.7373046875, 7.7373046875, 7.7373046875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -r4_abs.y, -r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (83) dp4 r0.xyz, c4.y, v1_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.8427734375, -4.8427734375, -4.8427734375, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, c4.y, v1_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (84) dp4 r0.xyz, r4_abs.zxyw, r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.82421875, 3.82421875, 3.82421875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, r4_abs.zxyw, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (85) dp4 r0.xyz, -c4_abs.zxyw, v1.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -6.0654296875, -6.0654296875, -6.0654296875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -c4_abs.zxyw, v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (86) dp4_pp r0.xyz, -r4.zxyw, -r5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.2978515625, 1.2978515625, 1.2978515625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -r4.zxyw, -r5_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (87) dp4_pp r0.xyz, c4.zxyw, v1.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.5302734375, -2.5302734375, -2.5302734375, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, c4.zxyw, v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (88) dp4_pp r0.xyz, r4_abs.xw, r5_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.1826171875, 5.1826171875, 5.1826171875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, r4_abs.xw, r5_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (89) dp4_pp r0.xyz, c4_abs.xw, -v1_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.59375, -1.59375, -1.59375, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, c4_abs.xw, -v1_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (90) dp4 r0.xyz, -r4_abs.xw, r5_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.7138671875, -4.7138671875, -4.7138671875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -r4_abs.xw, r5_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (91) dp4 r0.xyz, -v0_abs.xw, c5.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.1787109375, 0.1787109375, 0.1787109375, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -v0_abs.xw, c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (92) dp4_pp r0.xyz, -v0_abs.wzx, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.1201171875, -1.1201171875, -1.1201171875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -v0_abs.wzx, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (93) dp4 r0.xyz, c4_abs.wzx, v1_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.921875, 2.921875, 2.921875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, c4_abs.wzx, v1_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (94) dp4_pp r0.xyz, -v0.wzx, -c5.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.2900390625, -2.2900390625, -2.2900390625, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -v0.wzx, -c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (95) dp4 r0.xyz, r4_abs.yzwy, -r5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.71875, -1.71875, -1.71875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, r4_abs.yzwy, -r5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (96) dp4_pp r0.xyz, c4_abs.yzwy, -v1.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.8212890625, -0.8212890625, -0.8212890625, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, c4_abs.yzwy, -v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (97) dp4_pp r0.xyz, -v0_abs.yzwy, -c5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 6.0927734375, 6.0927734375, 6.0927734375, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -v0_abs.yzwy, -c5_abs.wzx
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

TESTCASE "DpN - Sat (1) dp3_pp r0.xyz, -c4_abs.x, -v1_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.966796875, 0.966796875, 0.966796875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -c4_abs.x, -v1_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (2) dp3_sat r0.xyz, -v0.y, c5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, -v0.y, c5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (3) dp3_sat_pp r0.xyz, r4_abs.zxyw, -r5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, r4_abs.zxyw, -r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (4) dp3_sat r0.xyz, c4_abs.xw, v1.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, c4_abs.xw, v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (5) dp3_pp r0.xyz, v0.wzx, -c5.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.7333984375, -0.7333984375, -0.7333984375, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, v0.wzx, -c5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (6) dp3 r0.xyz, -r4.yzwy, -r5.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.5009765625, 1.5009765625, 1.5009765625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -r4.yzwy, -r5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (7) dp3_sat_pp r0.xyz, -v0_abs, -c5_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -v0_abs, -c5_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (8) dp3_sat_pp r0.xyz, -r4_abs.x, -r5_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.908203125, 0.908203125, 0.908203125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -r4_abs.x, -r5_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (9) dp3 r0.xyz, -c4.y, v1_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.451171875, 3.451171875, 3.451171875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -c4.y, v1_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (10) dp3 r0.xyz, -c4.zxyw, v1_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.2265625, 0.2265625, 0.2265625, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -c4.zxyw, v1_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (11) dp3_pp r0.xyz, -r4.xw, -r5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.7509765625, 2.7509765625, 2.7509765625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -r4.xw, -r5_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (12) dp3_pp r0.xyz, -r4.wzx, -r5_abs.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.419921875, 2.419921875, 2.419921875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -r4.wzx, -r5_abs.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (13) dp3_sat_pp r0.xyz, v0_abs.x, c5.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, v0_abs.x, c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (14) dp3_sat_pp r0.xyz, r4_abs.yzwy, r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, r4_abs.yzwy, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (15) dp3_sat_pp r0.xyz, c4_abs.y, v1.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, c4_abs.y, v1.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (16) dp3_sat_pp r0.xyz, c4_abs.wzx, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, c4_abs.wzx, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (17) dp3 r0.xyz, c4_abs.yzwy, -v1_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.529296875, -3.529296875, -3.529296875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, c4_abs.yzwy, -v1_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (18) dp3_sat r0.xyz, -r4.x, -r5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, -r4.x, -r5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (19) dp3_sat r0.xyz, -r4.y, -r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, -r4.y, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (20) dp3 r0.xyz, -c4_abs, -v1_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.5361328125, 3.5361328125, 3.5361328125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -c4_abs, -v1_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (21) dp3_sat_pp r0.xyz, -v0_abs.zxyw, -c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -v0_abs.zxyw, -c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (22) dp3 r0.xyz, v0.xw, c5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.330078125, 1.330078125, 1.330078125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, v0.xw, c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (23) dp3_sat_pp r0.xyz, v0_abs.yzwy, -c5.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, v0_abs.yzwy, -c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (24) dp3 r0.xyz, v0_abs, -c5.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.2861328125, 0.2861328125, 0.2861328125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, v0_abs, -c5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (25) dp3 r0.xyz, -r4_abs.wzx, -r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.548828125, 0.548828125, 0.548828125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -r4_abs.wzx, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (26) dp3 r0.xyz, -v0_abs, c5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.8984375, -0.8984375, -0.8984375, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -v0_abs, c5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (27) dp3 r0.xyz, v0, c5_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.357421875, -1.357421875, -1.357421875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, v0, c5_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (28) dp3 r0.xyz, -v0, c5.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.85546875, 0.85546875, 0.85546875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -v0, c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (29) dp3 r0.xyz, c4_abs.x, -v1_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.908203125, -0.908203125, -0.908203125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, c4_abs.x, -v1_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (30) dp3_sat r0.xyz, c4.x, -v1.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, c4.x, -v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (31) dp3_sat r0.xyz, -r4_abs.x, r5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, -r4_abs.x, r5_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (32) dp3_pp r0.xyz, c4_abs.y, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.001953125, -1.001953125, -1.001953125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, c4_abs.y, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (33) dp3 r0.xyz, c4_abs.y, v1.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.56640625, 5.56640625, 5.56640625, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, c4_abs.y, v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (34) dp3_sat r0.xyz, v0_abs.y, c5.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, v0_abs.y, c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (35) dp3 r0.xyz, -v0.zxyw, c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.623046875, 0.623046875, 0.623046875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -v0.zxyw, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (36) dp3_sat r0.xyz, c4.zxyw, v1.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, c4.zxyw, v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (37) dp3_sat_pp r0.xyz, -r4.zxyw, -r5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -r4.zxyw, -r5_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (38) dp3 r0.xyz, c4_abs.zxyw, -v1.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3662109375, -1.3662109375, -1.3662109375, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, c4_abs.zxyw, -v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (39) dp3 r0.xyz, c4_abs.xw, v1_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.783203125, 2.783203125, 2.783203125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, c4_abs.xw, v1_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (40) dp3_pp r0.xyz, v0.xw, -c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.384765625, -2.384765625, -2.384765625, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, v0.xw, -c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (41) dp3_sat_pp r0.xyz, -v0_abs.xw, c5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -v0_abs.xw, c5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (42) dp3 r0.xyz, v0.xw, c5_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.2509765625, 4.2509765625, 4.2509765625, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, v0.xw, c5_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (43) dp3 r0.xyz, v0_abs.wzx, -c5_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.599609375, -2.599609375, -2.599609375, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, v0_abs.wzx, -c5_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (44) dp3_sat r0.xyz, c4_abs.wzx, v1_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, c4_abs.wzx, v1_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (45) dp3_sat_pp r0.xyz, v0_abs.wzx, -c5_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, v0_abs.wzx, -c5_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (46) dp3_pp r0.xyz, c4.yzwy, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.3828125, -0.3828125, -0.3828125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, c4.yzwy, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (47) dp3_pp r0.xyz, v0.yzwy, -c5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.212890625, -2.212890625, -2.212890625, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, v0.yzwy, -c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (48) dp3_sat r0.xyz, v0_abs.yzwy, -c5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, v0_abs.yzwy, -c5_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (49) dp4 r0.xyz, r4, r5"
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

TESTCASE "DpN - Sat (50) dp4_pp r0.xyz, -c4_abs.x, -v1_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.2890625, 1.2890625, 1.2890625, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -c4_abs.x, -v1_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (51) dp4_sat r0.xyz, -v0.y, c5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, -v0.y, c5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (52) dp4_sat_pp r0.xyz, r4_abs.zxyw, -r5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, r4_abs.zxyw, -r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (53) dp4_sat r0.xyz, c4_abs.xw, v1.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, c4_abs.xw, v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (54) dp4_pp r0.xyz, v0.wzx, -c5.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.4111328125, -0.4111328125, -0.4111328125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, v0.wzx, -c5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (55) dp4 r0.xyz, -r4.yzwy, -r5.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.9462890625, 1.9462890625, 1.9462890625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -r4.yzwy, -r5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (56) dp4_sat_pp r0.xyz, -v0_abs, -c5_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -v0_abs, -c5_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (57) dp4_sat_pp r0.xyz, -r4_abs.x, -r5_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -r4_abs.x, -r5_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (58) dp4 r0.xyz, -c4.y, v1_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.6220703125, 5.6220703125, 5.6220703125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -c4.y, v1_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (59) dp4 r0.xyz, -c4.zxyw, v1_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.265625, -0.265625, -0.265625, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -c4.zxyw, v1_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (60) dp4_pp r0.xyz, -r4.xw, -r5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.1044921875, 4.1044921875, 4.1044921875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -r4.xw, -r5_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (61) dp4_pp r0.xyz, -r4.wzx, -r5_abs.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8486328125, 1.8486328125, 1.8486328125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -r4.wzx, -r5_abs.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (62) dp4_sat_pp r0.xyz, v0_abs.x, c5.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, v0_abs.x, c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (63) dp4_sat_pp r0.xyz, r4_abs.yzwy, r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, r4_abs.yzwy, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (64) dp4_sat_pp r0.xyz, c4_abs.y, v1.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, c4_abs.y, v1.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (65) dp4_sat_pp r0.xyz, c4_abs.wzx, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, c4_abs.wzx, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (66) dp4 r0.xyz, c4_abs.yzwy, -v1_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.7001953125, -5.7001953125, -5.7001953125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, c4_abs.yzwy, -v1_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (67) dp4_sat r0.xyz, -r4.x, -r5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, -r4.x, -r5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (68) dp4_sat r0.xyz, -r4.y, -r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, -r4.y, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (69) dp4 r0.xyz, -c4_abs, -v1_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.0283203125, 4.0283203125, 4.0283203125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -c4_abs, -v1_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (70) dp4_sat_pp r0.xyz, -v0_abs.zxyw, -c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -v0_abs.zxyw, -c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (71) dp4 r0.xyz, v0.xw, c5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.7294921875, 3.7294921875, 3.7294921875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, v0.xw, c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (72) dp4_sat_pp r0.xyz, v0_abs.yzwy, -c5.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, v0_abs.yzwy, -c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (73) dp4 r0.xyz, v0_abs, -c5.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.0673828125, -1.0673828125, -1.0673828125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, v0_abs, -c5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (74) dp4 r0.xyz, -r4_abs.wzx, -r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1201171875, 1.1201171875, 1.1201171875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -r4_abs.wzx, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (75) dp4 r0.xyz, -v0_abs, c5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.390625, -1.390625, -1.390625, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -v0_abs, c5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (76) dp4 r0.xyz, v0, c5_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0419921875, 1.0419921875, 1.0419921875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, v0, c5_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (77) dp4 r0.xyz, -v0, c5.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.5439453125, -1.5439453125, -1.5439453125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -v0, c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (78) dp4 r0.xyz, c4_abs.x, -v1_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.4794921875, -1.4794921875, -1.4794921875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, c4_abs.x, -v1_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (79) dp4_sat r0.xyz, c4.x, -v1.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, c4.x, -v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (80) dp4_sat r0.xyz, -r4_abs.x, r5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, -r4_abs.x, r5_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (81) dp4_pp r0.xyz, c4_abs.y, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1689453125, 1.1689453125, 1.1689453125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, c4_abs.y, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (82) dp4 r0.xyz, c4_abs.y, v1.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 7.7373046875, 7.7373046875, 7.7373046875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, c4_abs.y, v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (83) dp4_sat r0.xyz, v0_abs.y, c5.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, v0_abs.y, c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (84) dp4 r0.xyz, -v0.zxyw, c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.73046875, -0.73046875, -0.73046875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -v0.zxyw, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (85) dp4_sat r0.xyz, c4.zxyw, v1.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.5810546875, 0.5810546875, 0.5810546875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, c4.zxyw, v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (86) dp4_sat_pp r0.xyz, -r4.zxyw, -r5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -r4.zxyw, -r5_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (87) dp4 r0.xyz, c4_abs.zxyw, -v1.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.8740234375, -0.8740234375, -0.8740234375, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, c4_abs.zxyw, -v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (88) dp4 r0.xyz, c4_abs.xw, v1_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.1826171875, 5.1826171875, 5.1826171875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, c4_abs.xw, v1_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (89) dp4_pp r0.xyz, v0.xw, -c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.73828125, -3.73828125, -3.73828125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, v0.xw, -c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (90) dp4_sat_pp r0.xyz, -v0_abs.xw, c5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -v0_abs.xw, c5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (91) dp4 r0.xyz, v0.xw, c5_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.7431640625, 4.7431640625, 4.7431640625, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, v0.xw, c5_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (92) dp4 r0.xyz, v0_abs.wzx, -c5_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.921875, -2.921875, -2.921875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, v0_abs.wzx, -c5_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (93) dp4_sat r0.xyz, c4_abs.wzx, v1_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, c4_abs.wzx, v1_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (94) dp4_sat_pp r0.xyz, v0_abs.wzx, -c5_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, v0_abs.wzx, -c5_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (95) dp4_pp r0.xyz, c4.yzwy, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0625, 0.0625, 0.0625, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, c4.yzwy, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (96) dp4_pp r0.xyz, v0.yzwy, -c5.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.0419921875, -0.0419921875, -0.0419921875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, v0.yzwy, -c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (97) dp4_sat r0.xyz, v0_abs.yzwy, -c5_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, v0_abs.yzwy, -c5_abs.wzx
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

TESTCASE "dpN - Pred/Mask (1) (p0.x) dp3 r0.x, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -1.220703125, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp3 r0.x, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (2) (p0.y) dp3 r0.y, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp3 r0.y, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (3) (!p0.z) dp3 r0.z, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp3 r0.z, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (4) (!p0.w) dp3 r0.w, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -1.220703125
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp3 r0.w, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (5) dp3 r0.xz, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.220703125, 0.0, -1.220703125, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (6) (!p0) dp3 r0.yw, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, -1.220703125
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp3 r0.yw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (7) (p0.z) dp3 r0.xyw, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp3 r0.xyw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (8) (p0.y) dp3 r0.xyzw, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, -1.220703125, -1.220703125
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp3 r0.xyzw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (9) (!p0.y) dp3 r0, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, -1.220703125, -1.220703125
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) dp3 r0, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (10) (!p0.w) dp3 r0.x, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp3 r0.x, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (11) (!p0) dp3 r0.y, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp3 r0.y, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (12) dp3 r0.z, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -1.220703125, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.z, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (13) (p0.y) dp3 r0.w, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp3 r0.w, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (14) (p0) dp3 r0.xz, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -1.220703125, 0.0, -1.220703125, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) dp3 r0.xz, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (15) (p0.x) dp3 r0.yw, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, -1.220703125
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp3 r0.yw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (16) (!p0.x) dp3 r0.xyw, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, 0.0, -1.220703125
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) dp3 r0.xyw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (17) dp3 r0.xyzw, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, -1.220703125, -1.220703125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyzw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (18) (!p0.w) dp3 r0, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, -1.220703125, -1.220703125
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp3 r0, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (19) (!p0) dp3 r0.z, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, -1.220703125, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp3 r0.z, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (20) (!p0) dp3 r0.x, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp3 r0.x, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (21) (p0.z) dp3 r0.xz, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp3 r0.xz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (22) dp3 r0.y, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.y, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (23) dp3 r0.w, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -1.220703125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.w, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (24) dp3 r0.yw, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, -1.220703125
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.yw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (25) (p0.w) dp3 r0.xyw, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp3 r0.xyw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (26) (!p0) dp3 r0.xyzw, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, -1.220703125
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp3 r0.xyzw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (27) (!p0.z) dp3 r0.y, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp3 r0.y, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (28) (p0.x) dp3 r0, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp3 r0, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (29) (!p0.x) dp3 r0.w, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) dp3 r0.w, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (30) (p0) dp3 r0.xyw, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, 0.0, -1.220703125
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) dp3 r0.xyw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (31) dp3 r0.x, r4, r5"
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

TESTCASE "dpN - Pred/Mask (32) (!p0.x) dp3 r0.z, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) dp3 r0.z, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (33) (p0.y) dp3 r0.xz, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp3 r0.xz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (34) (p0.z) dp3 r0.yw, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp3 r0.yw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (35) (p0.x) dp3 r0.xyzw, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp3 r0.xyzw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (36) (p0.z) dp3 r0, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, -1.220703125, -1.220703125
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp3 r0, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (37) (p0.x) dp3 r0.y, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp3 r0.y, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (38) dp3 r0.xyw, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.220703125, -1.220703125, 0.0, -1.220703125
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (39) (!p0.z) dp3 r0.w, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -1.220703125
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp3 r0.w, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (40) (p0.w) dp3 r0.xz, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, -1.220703125, 0.0, -1.220703125, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp3 r0.xz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (41) (!p0.z) dp3 r0.x, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -1.220703125, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp3 r0.x, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (42) (!p0.y) dp3 r0.z, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, -1.220703125, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) dp3 r0.z, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (43) (p0.w) dp3 r0.yw, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp3 r0.yw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (44) (p0.w) dp3 r0.xyzw, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp3 r0.xyzw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (45) (p0.w) dp3 r0.y, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp3 r0.y, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (46) dp4 r0, r4, r5"
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

TESTCASE "dpN - Pred/Mask (47) (p0.x) dp4 r0.x, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.1787109375, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp4 r0.x, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (48) (p0.y) dp4 r0.y, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp4 r0.y, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (49) (!p0.z) dp4 r0.z, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp4 r0.z, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (50) (!p0.w) dp4 r0.w, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.1787109375
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp4 r0.w, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (51) dp4 r0.xz, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1787109375, 0.0, 1.1787109375, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (52) (!p0) dp4 r0.yw, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 1.1787109375
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp4 r0.yw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (53) (p0.z) dp4 r0.xyw, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp4 r0.xyw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (54) (p0.y) dp4 r0.xyzw, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 1.1787109375, 1.1787109375
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp4 r0.xyzw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (55) (!p0.y) dp4 r0, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 1.1787109375, 1.1787109375
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) dp4 r0, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (56) (!p0.w) dp4 r0.x, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp4 r0.x, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (57) (!p0) dp4 r0.y, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp4 r0.y, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (58) dp4 r0.z, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.1787109375, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.z, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (59) (p0.y) dp4 r0.w, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp4 r0.w, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (60) (p0) dp4 r0.xz, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.1787109375, 0.0, 1.1787109375, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) dp4 r0.xz, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (61) (p0.x) dp4 r0.yw, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 1.1787109375
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp4 r0.yw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (62) (!p0.x) dp4 r0.xyw, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 0.0, 1.1787109375
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) dp4 r0.xyw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (63) dp4 r0.xyzw, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 1.1787109375, 1.1787109375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyzw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (64) (!p0.w) dp4 r0, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 1.1787109375, 1.1787109375
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp4 r0, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (65) (!p0) dp4 r0.z, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.1787109375, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp4 r0.z, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (66) (!p0) dp4 r0.x, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp4 r0.x, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (67) (p0.z) dp4 r0.xz, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp4 r0.xz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (68) dp4 r0.y, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.y, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (69) dp4 r0.w, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.1787109375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.w, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (70) dp4 r0.yw, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 1.1787109375
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.yw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (71) (p0.w) dp4 r0.xyw, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp4 r0.xyw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (72) (!p0) dp4 r0.xyzw, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 1.1787109375
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp4 r0.xyzw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (73) (!p0.z) dp4 r0.y, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp4 r0.y, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (74) (p0.x) dp4 r0, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp4 r0, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (75) (!p0.x) dp4 r0.w, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) dp4 r0.w, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (76) (p0) dp4 r0.xyw, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 0.0, 1.1787109375
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) dp4 r0.xyw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (77) dp4 r0.x, r4, r5"
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

TESTCASE "dpN - Pred/Mask (78) (!p0.x) dp4 r0.z, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) dp4 r0.z, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (79) (p0.y) dp4 r0.xz, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp4 r0.xz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (80) (p0.z) dp4 r0.yw, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp4 r0.yw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (81) (p0.x) dp4 r0.xyzw, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp4 r0.xyzw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (82) (p0.z) dp4 r0, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 1.1787109375, 1.1787109375
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp4 r0, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (83) (p0.x) dp4 r0.y, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) dp4 r0.y, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (84) dp4 r0.xyw, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1787109375, 1.1787109375, 0.0, 1.1787109375
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (85) (!p0.z) dp4 r0.w, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.1787109375
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp4 r0.w, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (86) (p0.w) dp4 r0.xz, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 1.1787109375, 0.0, 1.1787109375, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp4 r0.xz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (87) (!p0.z) dp4 r0.x, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.1787109375, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp4 r0.x, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (88) (!p0.y) dp4 r0.z, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.1787109375, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) dp4 r0.z, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (89) (p0.w) dp4 r0.yw, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp4 r0.yw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (90) (p0.w) dp4 r0.xyzw, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp4 r0.xyzw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (91) (p0.w) dp4 r0.y, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp4 r0.y, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  288 tests generated.