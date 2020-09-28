// Tests for Instruction: lrp
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

TESTCASE "lrp - Standard (1) lrp_pp r0, -c4_abs.x, -r5.x, -v2_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -8.3310546875, -8.3310546875, -8.3310546875, -8.3310546875
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4_abs.x, -r5.x, -v2_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (2) lrp r0, v0_abs.y, -c5_abs.y, -c5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.09375, -0.09375, -0.09375, -0.09375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, v0_abs.y, -c5_abs.y, -c5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (3) lrp_pp r0, -r4.zxyw, v1_abs.zxyw, c6_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.361328125, 1.9326171875, 0.13671875, 0.728515625
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4.zxyw, v1_abs.zxyw, c6_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (4) lrp r0, -v0.xw, -v0.xw, r6.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.4501953125, 0.12109375, 0.12109375, 0.12109375
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -v0.xw, -v0.xw, r6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (5) lrp_pp r0, c4.wzx, v1.wzx, -v1.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.52734375, 2.05078125, 8.794921875, 8.794921875
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, c4.wzx, v1.wzx, -v1.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (6) lrp r0, v0_abs.yzwy, v0.yzwy, v0_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.908203125, -4.0078125, 1.8125, -0.908203125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, v0_abs.yzwy, v0.yzwy, v0_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (7) lrp_pp r0, -c4, c4.x, c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.9375, -1.9375, -2.65625, 0.84375
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4, c4.x, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (8) lrp_pp r0, -r4_abs.x, -r5_abs.y, -r6_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.6708984375, -4.6396484375, -4.17578125, -3.1552734375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4_abs.x, -r5_abs.y, -r6_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (9) lrp r0, c4.y, r5_abs, v2.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.9248046875, -2.3056640625, -2.6689453125, -3.5166015625
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, c4.y, r5_abs, v2.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (10) lrp_pp r0, -v0.zxyw, c5.xw, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.78125, 2.5546875, 1.287109375, 1.34375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -v0.zxyw, c5.xw, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (11) lrp_pp r0, -r4_abs.xw, v1_abs.wzx, -c6_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -7.4697265625, -5.419921875, -7.798828125, -7.798828125
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4_abs.xw, v1_abs.wzx, -c6_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (12) lrp r0, -v0_abs.wzx, -v0_abs.zxyw, -r6_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.072265625, 1.642578125, -1.4326171875, -0.7919921875
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -v0_abs.wzx, -v0_abs.zxyw, -r6_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (13) lrp r0, -c4.yzwy, -v1_abs.yzwy, -v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.03515625, -2.015625, 7.4453125, -0.03515625
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -c4.yzwy, -v1_abs.yzwy, -v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (14) lrp_pp r0, v0_abs.x, -v0_abs, -v0_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.1201171875, -0.2724609375, -1.4150390625, -1.6611328125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, v0_abs.x, -v0_abs, -v0_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (15) lrp_pp r0, c4_abs.zxyw, -c4_abs.y, -c4_abs.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.28125, -0.1513671875, -0.9951171875, -0.283203125
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, c4_abs.zxyw, -c4_abs.y, -c4_abs.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (16) lrp_pp r0, r4_abs.y, -v1_abs.xw, -c6"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.6748046875, -1.255859375, -1.3212890625, -1.2666015625
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, r4_abs.y, -v1_abs.xw, -c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (17) lrp r0, v0_abs, -v0.wzx, -r6.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.962890625, 1.685546875, 2.3125, 2.654296875
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, v0_abs, -v0.wzx, -r6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (18) lrp r0, c4_abs.x, v1.zxyw, v1.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.013671875, -3.052734375, -1.1171875, 2.5546875
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, c4_abs.x, v1.zxyw, v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (19) lrp r0, v0.y, v0.x, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.96875, -1.96875, -1.96875, -1.96875
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, v0.y, v0.x, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (20) lrp r0, -c4.yzwy, -c4, c4.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.9638671875, 2.794921875, -8.595703125, -1.8173828125
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -c4.yzwy, -c4, c4.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (21) lrp_pp r0, r4_abs.xw, -r5.yzwy, -r6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.607421875, -0.34375, -3.62890625, -1.36328125
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, r4_abs.xw, -r5.yzwy, -r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (22) lrp_pp r0, -c4.wzx, r5_abs.y, v2.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.30078125, -0.271484375, 1.2744140625, 1.607421875
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4.wzx, r5_abs.y, v2.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (23) lrp_pp r0, -v0_abs.yzwy, -c5.x, -c5.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.78125, -6.6171875, -7.0078125, -4.37109375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -v0_abs.yzwy, -c5.x, -c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (24) lrp_pp r0, v0.zxyw, v0_abs, -r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.044921875, 2.9169921875, 1.564453125, 1.96484375
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, v0.zxyw, v0_abs, -r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (25) lrp r0, r4.wzx, -v1.x, -c6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.03515625, 0.94140625, 0.853515625, 0.853515625
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, r4.wzx, -v1.x, -c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (26) lrp_pp r0, -c4, -v1.xw, v1_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.416015625, -1.298828125, -2.33203125, 2.69921875
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4, -v1.xw, v1_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (27) lrp r0, -v0.xw, -v0_abs.zxyw, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.4150390625, 0.84375, -2.990234375, 8.3828125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -v0.xw, -v0_abs.zxyw, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (28) lrp_pp r0, -r4_abs.y, -r5.wzx, r6_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.193359375, 0.7763671875, 0.4892578125, 1.166015625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4_abs.y, -r5.wzx, r6_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (29) lrp r0, -c4_abs, -r5_abs.zxyw, v2_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.2626953125, 2.9560546875, 4.525390625, 7.005859375
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -c4_abs, -r5_abs.zxyw, v2_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (30) lrp_pp r0, v0.x, -c5_abs.yzwy, -c5_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.20703125, -4.365234375, 2.3671875, -3.8046875
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, v0.x, -c5_abs.yzwy, -c5_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (31) lrp r0, -c4_abs.wzx, c4_abs.xw, -c4_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -9.10546875, -8.349609375, -9.4130859375, -9.4130859375
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -c4_abs.wzx, c4_abs.xw, -c4_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (32) lrp r0, -r4_abs.zxyw, -r5.x, r6.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -6.029296875, -1.6513671875, -4.9248046875, -7.798828125
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -r4_abs.zxyw, -r5.x, r6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (33) lrp r0, -c4_abs.yzwy, -r5.xw, -v2_abs.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.9560546875, -2.099609375, -1.6953125, -0.9130859375
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -c4_abs.yzwy, -r5.xw, -v2_abs.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (34) lrp r0, v0_abs.xw, -c5_abs, -c5_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.416015625, 0.2109375, 0.2421875, -2.359375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, v0_abs.xw, -c5_abs, -c5_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (35) lrp_pp r0, -r4, v1.y, c6.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.7587890625, 0.0556640625, 0.931640625, -3.333984375
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4, v1.y, c6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (36) lrp r0, -v0.x, v0.x, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.3017578125, -2.453125, -4.4814453125, -2.7861328125
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -v0.x, v0.x, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (37) lrp_pp r0, v0_abs.wzx, -v0_abs.y, v0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.384765625, -0.28125, -0.96875, -3.6630859375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, v0_abs.wzx, -v0_abs.y, v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (38) lrp r0, -c4.x, -c4.wzx, -c4"
	PASSBEGIN
	TESTBEGIN
	def c1, -5.4755859375, 2.3837890625, 2.2412109375, 5.6318359375
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -c4.x, -c4.wzx, -c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (39) lrp_pp r0, c4.y, v1_abs.y, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.59765625, 0.09375, -1.013671875, 2.5546875
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, c4.y, v1_abs.y, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (40) lrp r0, r4_abs.yzwy, -r5.zxyw, r6_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.5048828125, 1.888671875, -1.490234375, -1.2509765625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, r4_abs.yzwy, -r5.zxyw, r6_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (41) lrp_pp r0, -v0_abs.y, -v0.yzwy, -r6.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.2763671875, -2.865234375, 4.955078125, 2.2607421875
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -v0_abs.y, -v0.yzwy, -r6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (42) lrp r0, -c4.zxyw, r5_abs.wzx, -v2_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.27734375, 2.345703125, 1.6796875, -7.359375
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -c4.zxyw, r5_abs.wzx, -v2_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (43) lrp_pp r0, -c4_abs.xw, -v1_abs.x, v1.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.115234375, -1.78125, 3.4921875, 7.0078125
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4_abs.xw, -v1_abs.x, v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (44) lrp r0, v0_abs.wzx, c5_abs.zxyw, c5_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.59765625, 1.78125, -1.541015625, 0.919921875
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, v0_abs.wzx, c5_abs.zxyw, c5_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (45) lrp_pp r0, -r4.wzx, -r5.yzwy, -r6.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.740234375, 0.017578125, -3.7353515625, -1.2744140625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4.wzx, -r5.yzwy, -r6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (46) lrp r0, c4_abs.xw, -r5.yzwy, -v2_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.3896484375, 2.04296875, -1.927734375, 0.744140625
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, c4_abs.xw, -r5.yzwy, -v2_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (47) lrp_pp r0, -r4_abs.x, v1_abs, -c6_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -7.8671875, -4.544921875, -5.283203125, -7.005859375
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4_abs.x, v1_abs, -c6_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (48) lrp_pp r0, -v0_abs, v0.xw, -v0_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, -5.078125, -7.9296875, -6.009765625
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -v0_abs, v0.xw, -v0_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (49) lrp_pp r0, -c4.y, -c4, -c4.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.9599609375, 1.0, 1.6650390625, -1.8125
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4.y, -c4, -c4.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (50) lrp_pp r0, -c4_abs.wzx, v1_abs, -v1_abs.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, -8.23828125, -3.76953125, -4.912109375, -6.634765625
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -c4_abs.wzx, v1_abs, -v1_abs.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (51) lrp_pp r0, v0.yzwy, v0.wzx, -v0.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.1201171875, -2.0234375, -2.095703125, -1.6611328125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, v0.yzwy, v0.wzx, -v0.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (52) lrp r0, -c4_abs.zxyw, c4_abs.yzwy, c4_abs.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.96875, 1.6875, 1.8125, 0.96875
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -c4_abs.zxyw, c4_abs.yzwy, c4_abs.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (53) lrp r0, -c4.xw, -c4.y, c4.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.1513671875, -6.501953125, -7.29296875, -7.29296875
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -c4.xw, -c4.y, c4.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (54) lrp_pp r0, v0.yzwy, -v0.zxyw, -r6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.2568359375, 0.625, 0.5625, 4.6474609375
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, v0.yzwy, -v0.zxyw, -r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (55) lrp_pp r0, -r4_abs, -v1.yzwy, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.6396484375, -3.6533203125, -2.099609375, -4.400390625
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4_abs, -v1.yzwy, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (56) lrp r0, -v0_abs.yzwy, v0.y, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.2607421875, -2.732421875, -2.814453125, -2.2607421875
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -v0_abs.yzwy, v0.y, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (57) lrp_pp r0, -r4.yzwy, v1.xw, c6.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.7607421875, 1.837890625, -7.005859375, 1.2509765625
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, -r4.yzwy, v1.xw, c6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (58) lrp r0, v0_abs, -c5.wzx, -c5.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.34375, 0.46875, 1.78125, 1.78125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, v0_abs, -c5.wzx, -c5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (59) lrp r0, c4.x, c4.zxyw, -c4.xw"
	PASSBEGIN
	TESTBEGIN
	def c1, 9.1669921875, -1.5048828125, -3.4736328125, -8.94921875
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, c4.x, c4.zxyw, -c4.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (60) lrp_pp r0, r4_abs.wzx, -r5_abs.xw, r6_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.548828125, -3.27734375, -3.2509765625, -3.7353515625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, r4_abs.wzx, -r5_abs.xw, r6_abs
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (61) lrp r0, -v0.zxyw, v0, -v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.98828125, -2.845703125, -1.6044921875, -0.560546875
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -v0.zxyw, v0, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (62) lrp r0, -r4.x, v1.xw, -c6.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.9296875, 3.2509765625, 1.5556640625, 1.22265625
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -r4.x, v1.xw, -c6.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (63) lrp r0, -r4.y, r5_abs.zxyw, -r6_abs.wzx"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.4189453125, 1.7060546875, 0.0400390625, 1.2509765625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -r4.y, r5_abs.zxyw, -r6_abs.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (64) lrp r0, c4.zxyw, v1.x, v1.yzwy"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.2578125, 2.115234375, 4.37109375, -3.3046875
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, c4.zxyw, v1.x, v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Standard (65) lrp r0, -v0.yzwy, -v0_abs.yzwy, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.0, -2.181640625, -1.4609375, -0.8818359375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, -v0.yzwy, -v0_abs.yzwy, v0
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

TESTCASE "lrp - sat (1) lrp_pp r0, c4, r5, v2"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3173828125, -2.982421875, 2.470703125, 3.349609375
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (2) lrp_sat r0, v0, c5, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.09375, 0.0, 1.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (3) lrp_sat_pp r0, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (4) lrp r0, v0, v0, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.9482421875, -1.953125, 4.52734375, 4.19921875
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (5) lrp r0, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.78125, 0.09375, -0.46875, 1.34375
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (6) lrp r0, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.96875, -0.96875, -1.6875, 1.8125
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (7) lrp r0, c4, c4, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.96875, -0.96875, -1.6875, 1.8125
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (8) lrp_sat r0, c4, r5, v2"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (9) lrp r0, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3173828125, -2.982421875, 2.470703125, 3.349609375
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (10) lrp_sat_pp r0, r4, r5, r6"
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

TESTCASE "lrp - sat (11) lrp_pp r0, v0, c5, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.78125, 0.09375, -0.46875, 1.34375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (12) lrp_sat_pp r0, v0, v0, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (13) lrp_sat_pp r0, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.09375, 0.0, 1.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (14) lrp_sat_pp r0, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - sat (15) lrp_sat_pp r0, c4, c4, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, c4, c4, c4
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

TESTCASE "lrp - Pred/Mask (1) (!p0.x) lrp_pp r0.x, c4, r5, v2"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) lrp_pp r0.x, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (2) (!p0.y) lrp_sat r0.y, v0, c5, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) lrp_sat r0.y, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (3) (p0.z) lrp_sat_pp r0.z, r4, v1, c6"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) lrp_sat_pp r0.z, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (4) (p0.w) lrp r0.w, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) lrp r0.w, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (5) (p0) lrp_sat r0.xz, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) lrp_sat r0.xz, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (6) lrp_sat r0.yw, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.yw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (7) (p0.y) lrp r0.xyw, c4, c4, c4"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -1.96875, -0.96875, 0.0, 1.8125
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) lrp r0.xyw, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (8) (!p0.z) lrp_pp r0.xyzw, r4, r5, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, -1.3173828125, -2.982421875, 2.470703125, 3.349609375
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) lrp_pp r0.xyzw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (9) (!p0.w) lrp_sat_pp r0, c4, r5, v2"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 1.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) lrp_sat_pp r0, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (10) lrp_sat r0.x, v0, c5, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.x, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (11) lrp_pp r0.y, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -2.982421875, 0.0, 0.0
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.y, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (12) (!p0) lrp_pp r0.z, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 4.52734375, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0) lrp_pp r0.z, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (13) lrp_pp r0.w, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.34375
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.w, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (14) (!p0.z) lrp_pp r0.xz, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, -1.96875, 0.0, -1.6875, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) lrp_pp r0.xz, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (15) (!p0.z) lrp_sat_pp r0.yw, c4, c4, c4"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) lrp_sat_pp r0.yw, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (16) lrp_sat r0.xyw, c4, r5, v2"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.xyw, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (17) (!p0.w) lrp_sat r0.xyzw, v0, c5, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) lrp_sat r0.xyzw, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (18) (!p0.x) lrp_sat r0.w, r4, v1, c6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) lrp_sat r0.w, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (19) (!p0.w) lrp_sat r0.z, r4, r5, r6"
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
	(!p0.w) lrp_sat r0.z, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (20) (p0.y) lrp_sat r0.yw, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) lrp_sat r0.yw, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (21) (p0.x) lrp_pp r0.xyw, v0, c5, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) lrp_pp r0.xyw, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (22) (p0) lrp_sat_pp r0.xyzw, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) lrp_sat_pp r0.xyzw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (23) (p0.y) lrp_pp r0, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) lrp_pp r0, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (24) (p0.w) lrp r0.x, c4, c4, c4"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, -1.96875, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) lrp r0.x, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (25) (p0.x) lrp_sat_pp r0.y, c4, r5, v2"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) lrp_sat_pp r0.y, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (26) lrp_sat r0.xz, v0, v0, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl_texcoord0 v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.xz, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (27) (!p0.w) lrp r0.yw, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) lrp r0.yw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (28) (p0.z) lrp_pp r0, r4, v1, c6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) lrp_pp r0, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (29) (!p0.y) lrp_sat r0.x, r4, r5, r6"
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
	(!p0.y) lrp_sat r0.x, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (30) (!p0.w) lrp_sat r0.y, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) lrp_sat r0.y, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (31) lrp_pp r0.z, c4, c4, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -1.6875, 0.0
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.z, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (32) (p0.w) lrp_sat r0.xz, r4, v1, c6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) lrp_sat r0.xz, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (33) (p0) lrp_sat r0.w, c4, r5, v2"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0) lrp_sat r0.w, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (34) (p0.w) lrp_sat r0.xyw, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) lrp_sat r0.xyw, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (35) lrp_pp r0.xyzw, c4, r5, v2"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.3173828125, -2.982421875, 2.470703125, 3.349609375
	dcl_texcoord2 v2
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	lrp_pp r0.xyzw, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (36) (p0.x) lrp_pp r0, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -1.78125, 0.09375, -0.46875, 1.34375
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) lrp_pp r0, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (37) (!p0.z) lrp_sat r0.x, r4, v1, c6"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) lrp_sat r0.x, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (38) (!p0.z) lrp_sat_pp r0.y, r4, r5, r6"
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
	(!p0.z) lrp_sat_pp r0.y, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (39) (!p0.z) lrp_sat r0.z, v0, c5, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) lrp_sat r0.z, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (40) (!p0.z) lrp_sat_pp r0.z, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) lrp_sat_pp r0.z, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (41) (p0.z) lrp_pp r0.w, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.8125
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) lrp_pp r0.w, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (42) (p0.y) lrp r0.xz, c4, c4, c4"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) lrp r0.xz, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (43) (!p0.y) lrp r0.yw, r4, v1, c6"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, -2.982421875, 0.0, 3.349609375
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) lrp r0.yw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (44) (!p0) lrp_pp r0.xyw, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, -1.96875, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) lrp_pp r0.xyw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (45) (p0.z) lrp r0.xyzw, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -0.9482421875, -1.953125, 4.52734375, 4.19921875
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) lrp r0.xyzw, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (46) (!p0) lrp_sat r0.yw, r4, r5, r6"
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
	(!p0) lrp_sat r0.yw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (47) (!p0.z) lrp_sat_pp r0.y, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) lrp_sat_pp r0.y, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (48) (p0.z) lrp_pp r0.y, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) lrp_pp r0.y, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (49) (p0) lrp_sat r0.y, c4, c4, c4"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) lrp_sat r0.y, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (50) (!p0.y) lrp r0.z, c4, r5, v2"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) lrp r0.z, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (51) (p0.x) lrp_sat r0.xz, r4, r5, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) lrp_sat r0.xz, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (52) (p0.x) lrp_pp r0.xyzw, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -1.96875, -0.96875, -1.6875, 1.8125
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) lrp_pp r0.xyzw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (53) lrp_sat_pp r0, v0, c5, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.09375, 0.0, 1.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (54) (p0.x) lrp_pp r0, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) lrp_pp r0, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (55) lrp r0, c4, c4, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.96875, -0.96875, -1.6875, 1.8125
	mov r0, c0.y
	mov r2, c0.z
	lrp r0, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (56) (!p0.z) lrp_sat_pp r0.x, v0, v0, r6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) lrp_sat_pp r0.x, v0, v0, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (57) (p0.y) lrp_sat r0.x, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) lrp_sat r0.x, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (58) (p0.x) lrp_pp r0.x, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -1.96875, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) lrp_pp r0.x, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (59) (p0.x) lrp_sat_pp r0.z, v0, v0, v0"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) lrp_sat_pp r0.z, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (60) (!p0.w) lrp r0.w, r4, r5, r6"
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
	(!p0.w) lrp r0.w, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (61) (!p0.w) lrp r0.w, v0, c5, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.34375
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) lrp r0.w, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (62) (p0.y) lrp_pp r0.w, c4, c4, c4"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) lrp_pp r0.w, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (63) (!p0.x) lrp_sat r0.xz, c4, r5, v2"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) lrp_sat r0.xz, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (64) (p0.y) lrp_sat_pp r0.xz, v0, c5, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) lrp_sat_pp r0.xz, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (65) (p0.w) lrp_pp r0.yw, c4, r5, v2"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord2 v2
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) lrp_pp r0.yw, c4, r5, v2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (66) (!p0) lrp_sat r0.yw, v0, c5, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.09375, 0.0, 1.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) lrp_sat r0.yw, v0, c5, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (67) (p0.z) lrp r0.xyw, r4, r5, r6"
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
	(p0.z) lrp r0.xyw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (68) lrp_sat r0.xyw, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl_texcoord1 v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	lrp_sat r0.xyw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (69) (p0.w) lrp_sat_pp r0.xyw, c4, v1, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.09375, 0.0, 1.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) lrp_sat_pp r0.xyw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (70) (!p0.z) lrp r0.xyzw, r4, v1, c6"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, -1.3173828125, -2.982421875, 2.470703125, 3.349609375
	dcl_texcoord1 v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) lrp r0.xyzw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "lrp - Pred/Mask (71) lrp_sat_pp r0.xyzw, c4, c4, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	mov r0, c0.y
	mov r2, c0.w
	lrp_sat_pp r0.xyzw, c4, c4, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  154 tests generated.