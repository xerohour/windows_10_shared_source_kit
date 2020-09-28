// Tests for Instruction: mad
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
TESTCASE "mad - Standard (0) mad r0, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8818359375, -1.5595703125, 1.416015625, 1.310546875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (1) mad_pp r0, -c4_abs.x, -r5.x, -v2_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.1318359375, -5.1318359375, -5.1318359375, -5.1318359375
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4_abs.x, -r5.x, -v2_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (2) mad r0, v0_abs.y, -c5_abs.y, -c5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.1845703125, -0.1845703125, -0.1845703125, -0.1845703125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, v0_abs.y, -c5_abs.y, -c5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (3) mad_pp r0, -r4.zxyw, v1_abs.zxyw, c6_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.416015625, 5.1318359375, 1.5595703125, -1.310546875
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4.zxyw, v1_abs.zxyw, c6_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (4) mad r0, -v0.xw, -v0.xw, r6.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.2509765625, 2.16015625, 2.16015625, 2.16015625
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -v0.xw, -v0.xw, r6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (5) mad_pp r0, c4.wzx, v1.wzx, -v1.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.091796875, 1.259765625, 5.2880859375, 5.2880859375
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, c4.wzx, v1.wzx, -v1.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (6) mad r0, v0_abs.yzwy, v0.yzwy, v0_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0302734375, -1.16015625, 5.09765625, 0.0302734375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, v0_abs.yzwy, v0.yzwy, v0_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (7) mad_pp r0, -c4, c4.x, c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.8447265625, -2.8759765625, -4.291015625, 2.599609375
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4, c4.x, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (8) mad_pp r0, -r4_abs.x, -r5_abs.y, -r6_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.4404296875, -1.4404296875, -1.2841796875, -0.9404296875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4_abs.x, -r5_abs.y, -r6_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (9) mad r0, c4.y, r5_abs, v2.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.3505859375, -1.2158203125, -1.5791015625, -2.4267578125
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, c4.y, r5_abs, v2.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (10) mad_pp r0, -v0.zxyw, c5.xw, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.787109375, 2.7392578125, 0.8330078125, -1.091796875
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -v0.zxyw, c5.xw, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (11) mad_pp r0, -r4_abs.xw, v1_abs.wzx, -c6_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.2705078125, -2.474609375, -4.853515625, -4.853515625
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4_abs.xw, v1_abs.wzx, -c6_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (12) mad r0, -v0_abs.wzx, -v0_abs.zxyw, -r6_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.58984375, 2.697265625, 0.7822265625, 2.099609375
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -v0_abs.wzx, -v0_abs.zxyw, -r6_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (13) mad r0, -c4.yzwy, -v1_abs.yzwy, -v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.6904296875, 0.990234375, 4.216796875, 1.6904296875
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -c4.yzwy, -v1_abs.yzwy, -v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (14) mad_pp r0, v0_abs.x, -v0_abs, -v0_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.6884765625, -3.5947265625, -5.291015625, -5.537109375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, v0_abs.x, -v0_abs, -v0_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (15) mad_pp r0, c4_abs.zxyw, -c4_abs.y, -c4_abs.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.603515625, -3.7197265625, -2.7509765625, -3.568359375
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, c4_abs.zxyw, -c4_abs.y, -c4_abs.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (16) mad_pp r0, r4_abs.y, -v1_abs.xw, -c6"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.1005859375, 0.1669921875, -1.9267578125, -0.1767578125
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, r4_abs.y, -v1_abs.xw, -c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (17) mad r0, v0_abs, -v0.wzx, -r6.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.193359375, 3.259765625, 4.791015625, 4.693359375
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, v0_abs, -v0.wzx, -r6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (18) mad r0, c4_abs.x, v1.zxyw, v1.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.8291015625, -3.9755859375, 1.5283203125, 2.7392578125
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	mad r0, c4_abs.x, v1.zxyw, v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (19) mad r0, v0.y, v0.x, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.0615234375, -0.0615234375, -0.0615234375, -0.0615234375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, v0.y, v0.x, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (20) mad r0, -c4.yzwy, -c4, c4.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.7197265625, -0.052734375, -5.02734375, -3.724609375
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -c4.yzwy, -c4, c4.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (21) mad_pp r0, r4_abs.xw, -r5.yzwy, -r6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.2841796875, 2.318359375, -0.966796875, 1.298828125
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, r4_abs.xw, -r5.yzwy, -r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (22) mad_pp r0, -c4.wzx, r5_abs.y, v2.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.638671875, 0.783203125, -0.9404296875, -1.2841796875
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4.wzx, r5_abs.y, v2.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (23) mad_pp r0, -v0_abs.yzwy, -c5.x, -c5.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0556640625, -4.349609375, -4.572265625, -3.0693359375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -v0_abs.yzwy, -c5.x, -c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (24) mad_pp r0, v0.zxyw, v0_abs, -r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.697265625, -0.2822265625, -0.009765625, 4.91015625
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, v0.zxyw, v0_abs, -r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (25) mad r0, r4.wzx, -v1.x, -c6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.697265625, -1.537109375, -2.0380859375, -2.0380859375
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad r0, r4.wzx, -v1.x, -c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (26) mad_pp r0, -c4, -v1.xw, v1_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.6005859375, -1.2080078125, -2.173828125, 2.529296875
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4, -v1.xw, v1_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (27) mad r0, -v0.xw, -v0_abs.zxyw, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.291015625, 2.599609375, 0.068359375, 5.09765625
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -v0.xw, -v0_abs.zxyw, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (28) mad_pp r0, -r4_abs.y, -r5.wzx, r6_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.7705078125, 0.1708984375, -0.6005859375, -0.2568359375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4_abs.y, -r5.wzx, r6_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (29) mad r0, -c4_abs, -r5_abs.zxyw, v2_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.0478515625, 2.3505859375, 1.783203125, 4.060546875
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -c4_abs, -r5_abs.zxyw, v2_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (30) mad_pp r0, v0.x, -c5_abs.yzwy, -c5_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.2841796875, -0.8583984375, 2.5517578125, -1.1591796875
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, v0.x, -c5_abs.yzwy, -c5_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (31) mad r0, -c4_abs.wzx, c4_abs.xw, -c4_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.537109375, -5.02734375, -5.537109375, -5.537109375
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -c4_abs.wzx, c4_abs.xw, -c4_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (32) mad r0, -r4_abs.zxyw, -r5.x, r6.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.130859375, -2.8818359375, -3.3505859375, -4.853515625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -r4_abs.zxyw, -r5.x, r6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (33) mad r0, -c4_abs.yzwy, -r5.xw, -v2_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.3505859375, 0.642578125, 0.966796875, 0.1767578125
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -c4_abs.yzwy, -r5.xw, -v2_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (34) mad r0, v0_abs.xw, -c5_abs, -c5_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.6005859375, -0.638671875, -2.193359375, -2.529296875
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, v0_abs.xw, -c5_abs, -c5_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (35) mad_pp r0, -r4, v1.y, c6.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.4404296875, -1.0341796875, -0.966796875, -1.294921875
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4, v1.y, c6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (36) mad r0, -v0.x, v0.x, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.5009765625, -5.3447265625, -3.2509765625, -5.0009765625
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -v0.x, v0.x, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (37) mad_pp r0, v0_abs.wzx, -v0_abs.y, v0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.443359375, -3.603515625, -2.8759765625, -0.0947265625
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, v0_abs.wzx, -v0_abs.y, v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (38) mad r0, -c4.x, -c4.wzx, -c4"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.599609375, 4.291015625, 5.5634765625, 2.0634765625
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -c4.x, -c4.wzx, -c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (39) mad_pp r0, c4.y, v1_abs.y, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.8720703125, 0.0029296875, -0.5595703125, 1.2529296875
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, c4.y, v1_abs.y, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (40) mad r0, r4_abs.yzwy, -r5.zxyw, r6_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.0791015625, 4.630859375, 1.455078125, 0.3232421875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, r4_abs.yzwy, -r5.zxyw, r6_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (41) mad_pp r0, -v0_abs.y, -v0.yzwy, -r6.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.1865234375, -2.259765625, 3.380859375, 0.6865234375
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -v0_abs.y, -v0.yzwy, -r6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (42) mad r0, -c4.zxyw, r5_abs.wzx, -v2_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.798828125, -0.5458984375, 0.2568359375, -4.697265625
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -c4.zxyw, r5_abs.wzx, -v2_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (43) mad_pp r0, -c4_abs.xw, -v1_abs.x, v1.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.0380859375, 1.447265625, 3.322265625, 4.572265625
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4_abs.xw, -v1_abs.x, v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (44) mad r0, v0_abs.wzx, c5_abs.zxyw, c5_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.630859375, 4.787109375, 1.9658203125, 4.4267578125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, v0_abs.wzx, c5_abs.zxyw, c5_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (45) mad_pp r0, -r4.wzx, -r5.yzwy, -r6.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.794921875, 1.916015625, -1.5205078125, 0.9404296875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4.wzx, -r5.yzwy, -r6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (46) mad r0, c4_abs.xw, -r5.yzwy, -v2_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.8095703125, -0.619140625, -3.060546875, -1.294921875
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, c4_abs.xw, -r5.yzwy, -v2_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (47) mad_pp r0, -r4_abs.x, v1_abs, -c6_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.9755859375, -1.6533203125, -2.3916015625, -4.1142578125
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4_abs.x, v1_abs, -c6_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (48) mad_pp r0, -v0_abs, v0.xw, -v0_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.9072265625, -3.443359375, -4.87109375, -4.25390625
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -v0_abs, v0.xw, -v0_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (49) mad_pp r0, -c4.y, -c4, -c4.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.5947265625, 2.9072265625, 2.603515625, -3.568359375
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4.y, -c4, -c4.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (50) mad_pp r0, -c4_abs.wzx, v1_abs, -v1_abs.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.009765625, -1.501953125, -2.2666015625, -3.9892578125
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4_abs.wzx, v1_abs, -v1_abs.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (51) mad_pp r0, v0.yzwy, v0.wzx, -v0.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.212890625, 1.03515625, -5.380859375, 0.0947265625
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, v0.yzwy, v0.wzx, -v0.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (52) mad r0, -c4_abs.zxyw, c4_abs.yzwy, c4_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.666015625, -1.634765625, 0.056640625, -0.787109375
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -c4_abs.zxyw, c4_abs.yzwy, c4_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (53) mad r0, -c4.xw, -c4.y, c4.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.7197265625, -3.443359375, -3.724609375, -3.724609375
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -c4.xw, -c4.y, c4.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (54) mad_pp r0, v0.yzwy, -v0.zxyw, -r6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.166015625, -1.853515625, 3.224609375, 3.224609375
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, v0.yzwy, -v0.zxyw, -r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (55) mad_pp r0, -r4_abs, -v1.yzwy, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.4404296875, -2.0791015625, 0.642578125, -1.455078125
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4_abs, -v1.yzwy, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (56) mad r0, -v0_abs.yzwy, v0.y, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.6865234375, 0.009765625, 0.130859375, -0.6865234375
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -v0_abs.yzwy, v0.y, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (57) mad_pp r0, -r4.yzwy, v1.xw, c6.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.8505859375, 2.892578125, -4.060546875, -0.3232421875
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4.yzwy, v1.xw, c6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (58) mad r0, v0_abs, -c5.wzx, -c5.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.9892578125, 0.9228515625, 4.787109375, 5.009765625
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, v0_abs, -c5.wzx, -c5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (59) mad r0, c4.x, c4.zxyw, -c4.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.291015625, 2.0634765625, 0.0947265625, -5.380859375
	mov r0, c0.y
	mov r2, c0.z
	mad r0, c4.x, c4.zxyw, -c4.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (60) mad_pp r0, r4_abs.wzx, -r5_abs.xw, r6_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.603515625, -0.798828125, -2.0205078125, -1.5205078125
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, r4_abs.wzx, -r5_abs.xw, r6_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (61) mad r0, -v0.zxyw, v0, -v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.353515625, -0.9384765625, -0.666015625, -2.31640625
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -v0.zxyw, v0, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (62) mad r0, -r4.x, v1.xw, -c6.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.0380859375, 2.0205078125, 3.7705078125, 4.1142578125
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -r4.x, v1.xw, -c6.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (63) mad r0, -r4.y, r5_abs.zxyw, -r6_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.6708984375, 1.1005859375, -1.5341796875, -0.3232421875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -r4.y, r5_abs.zxyw, -r6_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (64) mad r0, c4.zxyw, v1.x, v1.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.099609375, 3.0380859375, 3.0693359375, -3.134765625
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	mad r0, c4.zxyw, v1.x, v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (65) mad r0, -v0.yzwy, -v0_abs.yzwy, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.9072265625, -3.81640625, 1.59765625, 0.8740234375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -v0.yzwy, -v0_abs.yzwy, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * sat * pp
TESTCASE "mad - sat (0) mad r0, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8818359375, -1.5595703125, 1.416015625, 1.310546875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (1) mad_pp r0, c4, r5, v2"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8818359375, -1.5595703125, 1.416015625, 1.310546875
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (2) mad_sat r0, v0, c5, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0029296875, 0.322265625, 1.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (3) mad_sat_pp r0, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (4) mad r0, v0, v0, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.2509765625, -0.5302734375, 3.47265625, 2.16015625
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (5) mad r0, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.7255859375, 0.0029296875, 0.322265625, 3.779296875
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	mad r0, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (6) mad r0, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.9072265625, -0.0302734375, 1.16015625, 5.09765625
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (7) mad r0, c4, c4, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.9072265625, -0.0302734375, 1.16015625, 5.09765625
	mov r0, c0.y
	mov r2, c0.z
	mad r0, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (8) mad_sat r0, c4, r5, v2"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (9) mad r0, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8818359375, -1.5595703125, 1.416015625, 1.310546875
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad r0, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (10) mad_sat_pp r0, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (11) mad_pp r0, v0, c5, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.7255859375, 0.0029296875, 0.322265625, 3.779296875
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (12) mad_sat_pp r0, v0, v0, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (13) mad_sat_pp r0, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0029296875, 0.322265625, 1.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (14) mad_sat_pp r0, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (15) mad_sat_pp r0, c4, c4, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// masks & predication
TESTCASE "mad - Pred/Mask (0) mad r0, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8818359375, -1.5595703125, 1.416015625, 1.310546875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (1) (!p0.x) mad_pp r0.x, c4, r5, v2"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) mad_pp r0.x, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (2) (!p0.y) mad_sat r0.y, v0, c5, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) mad_sat r0.y, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (3) (p0.z) mad_sat_pp r0.z, r4, v1, c6"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) mad_sat_pp r0.z, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (4) (p0.w) mad r0.w, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) mad r0.w, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (5) (p0) mad_sat r0.xz, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.0, 0.0, 0.322265625, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) mad_sat r0.xz, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (6) mad_sat r0.yw, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.yw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (7) (p0.y) mad r0.xyw, c4, c4, c4"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.9072265625, -0.0302734375, 0.0, 5.09765625
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) mad r0.xyw, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (8) (!p0.z) mad_pp r0.xyzw, r4, r5, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 1.8818359375, -1.5595703125, 1.416015625, 1.310546875
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) mad_pp r0.xyzw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (9) (!p0.w) mad_sat_pp r0, c4, r5, v2"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) mad_sat_pp r0, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (10) mad_sat r0.x, v0, c5, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.x, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (11) mad_pp r0.y, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.5595703125, 0.0, 0.0
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.y, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (12) (!p0) mad_pp r0.z, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 3.47265625, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0) mad_pp r0.z, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (13) mad_pp r0.w, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 3.779296875
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.w, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (14) (!p0.z) mad_pp r0.xz, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 1.9072265625, 0.0, 1.16015625, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) mad_pp r0.xz, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (15) (!p0.z) mad_sat_pp r0.yw, c4, c4, c4"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) mad_sat_pp r0.yw, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (16) mad_sat r0.xyw, c4, r5, v2"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 1.0
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.xyw, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (17) (!p0.w) mad_sat r0.xyzw, v0, c5, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) mad_sat r0.xyzw, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (18) (!p0.x) mad_sat r0.w, r4, v1, c6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) mad_sat r0.w, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (19) (!p0.w) mad_sat r0.z, r4, r5, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) mad_sat r0.z, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (20) (p0.y) mad_sat r0.yw, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) mad_sat r0.yw, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (21) (p0.x) mad_pp r0.xyw, v0, c5, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) mad_pp r0.xyw, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (22) (p0) mad_sat_pp r0.xyzw, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) mad_sat_pp r0.xyzw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (23) (p0.y) mad_pp r0, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) mad_pp r0, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (24) (p0.w) mad r0.x, c4, c4, c4"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 1.9072265625, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) mad r0.x, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (25) (p0.x) mad_sat_pp r0.y, c4, r5, v2"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) mad_sat_pp r0.y, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (26) mad_sat r0.xz, v0, v0, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 0.0
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.xz, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (27) (!p0.w) mad r0.yw, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) mad r0.yw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (28) (p0.z) mad_pp r0, r4, v1, c6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) mad_pp r0, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (29) (!p0.y) mad_sat r0.x, r4, r5, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) mad_sat r0.x, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (30) (!p0.w) mad_sat r0.y, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) mad_sat r0.y, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (31) mad_pp r0.z, c4, c4, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.16015625, 0.0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.z, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (32) (p0.w) mad_sat r0.xz, r4, v1, c6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) mad_sat r0.xz, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (33) (p0) mad_sat r0.w, c4, r5, v2"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0) mad_sat r0.w, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (34) (p0.w) mad_sat r0.xyw, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) mad_sat r0.xyw, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (35) mad_pp r0.xyzw, c4, r5, v2"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8818359375, -1.5595703125, 1.416015625, 1.310546875
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.xyzw, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (36) (p0.x) mad_pp r0, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.7255859375, 0.0029296875, 0.322265625, 3.779296875
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) mad_pp r0, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (37) (!p0.z) mad_sat r0.x, r4, v1, c6"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) mad_sat r0.x, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (38) (!p0.z) mad_sat_pp r0.y, r4, r5, r6"
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
	(!p0.z) mad_sat_pp r0.y, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (39) (!p0.z) mad_sat r0.z, v0, c5, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) mad_sat r0.z, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (40) (!p0.z) mad_sat_pp r0.z, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.322265625, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) mad_sat_pp r0.z, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (41) (p0.z) mad_pp r0.w, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 5.09765625
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) mad_pp r0.w, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (42) (p0.y) mad r0.xz, c4, c4, c4"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) mad r0.xz, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (43) (!p0.y) mad r0.yw, r4, v1, c6"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, -1.5595703125, 0.0, 1.310546875
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) mad r0.yw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (44) (!p0) mad_pp r0.xyw, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 1.9072265625, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) mad_pp r0.xyw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (45) (p0.z) mad r0.xyzw, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 2.2509765625, -0.5302734375, 3.47265625, 2.16015625
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) mad r0.xyzw, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (46) (!p0) mad_sat r0.yw, r4, r5, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(!p0) mad_sat r0.yw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (47) (!p0.z) mad_sat_pp r0.y, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) mad_sat_pp r0.y, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (48) (p0.z) mad_pp r0.y, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) mad_pp r0.y, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (49) (p0) mad_sat r0.y, c4, c4, c4"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) mad_sat r0.y, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (50) (!p0.y) mad r0.z, c4, r5, v2"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) mad r0.z, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (51) (p0.x) mad_sat r0.xz, r4, r5, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) mad_sat r0.xz, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (52) (p0.x) mad_pp r0.xyzw, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.9072265625, -0.0302734375, 1.16015625, 5.09765625
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) mad_pp r0.xyzw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (53) mad_sat_pp r0, v0, c5, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0029296875, 0.322265625, 1.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (54) (p0.x) mad_pp r0, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) mad_pp r0, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (55) mad r0, c4, c4, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.9072265625, -0.0302734375, 1.16015625, 5.09765625
	mov r0, c0.y
	mov r2, c0.z
	mad r0, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (56) (!p0.z) mad_sat_pp r0.x, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) mad_sat_pp r0.x, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (57) (p0.y) mad_sat r0.x, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) mad_sat r0.x, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (58) (p0.x) mad_pp r0.x, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.9072265625, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) mad_pp r0.x, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (59) (p0.x) mad_sat_pp r0.z, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) mad_sat_pp r0.z, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (60) (!p0.w) mad r0.w, r4, r5, r6"
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
	(!p0.w) mad r0.w, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (61) (!p0.w) mad r0.w, v0, c5, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 3.779296875
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) mad r0.w, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (62) (p0.y) mad_pp r0.w, c4, c4, c4"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) mad_pp r0.w, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (63) (!p0.x) mad_sat r0.xz, c4, r5, v2"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) mad_sat r0.xz, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (64) (p0.y) mad_sat_pp r0.xz, v0, c5, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.0, 0.0, 0.322265625, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) mad_sat_pp r0.xz, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (65) (p0.w) mad_pp r0.yw, c4, r5, v2"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) mad_pp r0.yw, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (66) (!p0) mad_sat r0.yw, v0, c5, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0029296875, 0.0, 1.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) mad_sat r0.yw, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (67) (p0.z) mad r0.xyw, r4, r5, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) mad r0.xyw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (68) mad_sat r0.xyw, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 1.0
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.xyw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (69) (p0.w) mad_sat_pp r0.xyw, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 1.0, 0.0029296875, 0.0, 1.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) mad_sat_pp r0.xyw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (70) (!p0.z) mad r0.xyzw, r4, v1, c6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 1.8818359375, -1.5595703125, 1.416015625, 1.310546875
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) mad r0.xyzw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (71) mad_sat_pp r0.xyzw, c4, c4, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.xyzw, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  154 tests generated.