// Tests for Instruction: mad
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
TESTCASE "mad - Standard (0) mad r0, r4, r5, r6";
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

TESTCASE "mad - Standard (1) mad_pp r0, -v0.x, c5.x, -r6.x";
	PASSBEGIN
	TESTBEGIN
	def c1, 3.37841796875, 3.37841796875, 3.37841796875, 3.37841796875
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -v0.x, c5.x, -r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (2) mad r0, r4.y, -v1.y, -c6.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.51416015625, 1.51416015625, 1.51416015625, 1.51416015625
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad r0, r4.y, -v1.y, -c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (3) mad_pp r0, -c4.zxyw, -c4.zxyw, v0.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, 3.69140625, 4.8603515625, 1.4228515625, 4.19140625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4.zxyw, -c4.zxyw, v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (4) mad r0, -t0.xw, r5.xw, c6.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -5.1318359375, -3.560546875, -3.560546875, -3.560546875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -t0.xw, r5.xw, c6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (5) mad_pp r0, c4.wzx, t1.wzx, c4.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 4.248046875, -0.896484375, 1.5380859375, 1.5380859375
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, c4.wzx, t1.wzx, c4.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (6) mad r0, r4.yzwy, c5.yzwy, t2.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.5595703125, 1.416015625, 1.310546875, -1.5595703125
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad r0, r4.yzwy, c5.yzwy, t2.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (7) mad_pp r0, -v0, -c5.x, v0.y";
	PASSBEGIN
	TESTBEGIN
	def c1, -1.26904296875, -0.37841796875, -1.0185546875, -1.1298828125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -v0, -c5.x, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (8) mad r0, t0.x, -t0, -c6.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, -4.5009765625, -0.2822265625, -1.853515625, 4.693359375
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0.x, -t0, -c6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (9) mad_pp r0, -c4.y, v1.zxyw, -v1";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.66357421875, 0.81591796875, -0.18896484375, -0.02099609375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4.y, v1.zxyw, -v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (10) mad r0, t0.zxyw, r5.y, t0.x";
	PASSBEGIN
	TESTBEGIN
	def c1, -2.126953125, -2.1533203125, -2.0595703125, -1.798828125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0.zxyw, r5.y, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (11) mad_pp r0, -v0.xw, -v0.wzx, -v0.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.40771484375, -0.0791015625, -0.01416015625, 0.40771484375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -v0.xw, -v0.wzx, -v0.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (12) mad_pp r0, t0.wzx, -t0.xw, -t0.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 5.537109375, 1.24609375, 1.755859375, 1.755859375
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, t0.wzx, -t0.xw, -t0.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (13) mad_pp r0, -r4.yzwy, -r5.yzwy, -r6.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0341796875, 0.166015625, 4.060546875, 1.5341796875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4.yzwy, -r5.yzwy, -r6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (14) mad r0, v0.wzx, -c5, r6.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.1455078125, -1.5478515625, -1.00732421875, -2.79150390625
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad r0, v0.wzx, -c5, r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (15) mad_pp r0, -r4, v1.zxyw, c6.x";
	PASSBEGIN
	TESTBEGIN
	def c1, -1.16357421875, -0.76220703125, -1.5458984375, -2.8427734375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4, v1.zxyw, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (16) mad r0, c4.x, c4.y, -v0";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.9228515625, 1.4228515625, 1.0634765625, 1.0009765625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, c4.x, c4.y, -v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (17) mad_pp r0, t0.y, -r5.x, -c6.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, -2.3505859375, -0.1005859375, -0.2568359375, -0.6005859375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, t0.y, -r5.x, -c6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (18) mad r0, -c4.zxyw, -t1, -c4";
	PASSBEGIN
	TESTBEGIN
	def c1, 4.974609375, 0.7841796875, 2.1416015625, 0.623046875
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -c4.zxyw, -t1, -c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (19) mad_pp r0, -r4.xw, -c5.y, -t2.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.8095703125, 1.794921875, 1.638671875, 1.294921875
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4.xw, -c5.y, -t2.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (20) mad r0, v0.yzwy, c5.zxyw, -v0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.21142578125, -2.4873046875, -0.8994140625, -0.33349609375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, v0.yzwy, c5.zxyw, -v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (21) mad_pp r0, -t0, t0.xw, c6.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -5.0009765625, 2.380859375, 1.43359375, -4.91015625
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -t0, t0.xw, c6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (22) mad r0, c4.x, -v1.wzx, v1.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.36962890625, 0.50830078125, 1.80029296875, 1.80029296875
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad r0, c4.x, -v1.wzx, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (23) mad_pp r0, -t0.y, -r5, -t0.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.0869140625, 1.5966796875, 2.4228515625, 0.6669921875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -t0.y, -r5, -t0.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (24) mad r0, -t0.xw, t0.x, t0.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.0634765625, 1.880859375, 1.599609375, 1.599609375
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -t0.xw, t0.x, t0.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (25) mad r0, -v0.wzx, -v0.yzwy, v0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.42333984375, 1.6962890625, 1.87646484375, 1.46118164063
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, -v0.wzx, -v0.yzwy, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (26) mad r0, v0.yzwy, v0.xw, -v0.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.007568359375, 0.2802734375, 0.3369140625, -0.04541015625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, v0.yzwy, v0.xw, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (27) mad r0, -r4.zxyw, r5.xw, r6.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.380859375, 1.0205078125, -0.1669921875, -3.560546875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -r4.zxyw, r5.xw, r6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (28) mad_pp r0, -v0, c5.wzx, -r6.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.94775390625, 1.85205078125, 2.9716796875, 2.7392578125
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -v0, c5.wzx, -r6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (29) mad_pp r0, -c4.y, c4.yzwy, v0.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0458984375, -0.728515625, 2.662109375, -0.0322265625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4.y, c4.yzwy, v0.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (30) mad_pp r0, -r4.zxyw, -v1.x, -c6.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.1220703125, -0.62841796875, 0.26220703125, 2.7392578125
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4.zxyw, -v1.x, -c6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (31) mad_pp r0, c4.xw, -t1.zxyw, c4.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.8916015625, 2.259765625, -1.138671875, -3.404296875
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, c4.xw, -t1.zxyw, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (32) mad r0, t0.yzwy, -r5, c6";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.3505859375, -1.310546875, 1.474609375, 0.1767578125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0.yzwy, -r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (33) mad_pp r0, -r4.x, c5.zxyw, t2.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.5478515625, -4.6318359375, -0.9404296875, 1.5205078125
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4.x, c5.zxyw, t2.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (34) mad r0, v0.wzx, -c5.y, -v0.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.5693359375, -0.9228515625, -0.99853515625, -0.57666015625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, v0.wzx, -c5.y, -v0.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (35) mad_pp r0, t0.zxyw, t0.wzx, c6";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -4.68359375, 1.853515625, 2.5322265625, -4.693359375
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, t0.zxyw, t0.wzx, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (36) mad r0, c4.yzwy, v1, -v1.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.75341796875, -0.7509765625, -0.2470703125, -1.32275390625
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad r0, c4.yzwy, v1, -v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (37) mad_pp r0, -t0, -r5.yzwy, -t0";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.7841796875, 1.4228515625, -0.580078125, -1.642578125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -t0, -r5.yzwy, -t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (38) mad r0, t0.x, -t0.yzwy, t0.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.8759765625, -4.291015625, 2.599609375, -2.8759765625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0.x, -t0.yzwy, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (39) mad_pp r0, -r4.y, -r5.wzx, -r6.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.3232421875, 2.0791015625, 3.3505859375, 3.3505859375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4.y, -r5.wzx, -r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (40) mad_pp r0, v0.zxyw, -c5.xw, -r6.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.9716796875, -1.94775390625, 0.47412109375, 0.2509765625
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, v0.zxyw, -c5.xw, -r6.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (41) mad_pp r0, r4.xw, -v1, -c6";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 3.37841796875, 1.3837890625, -1.0498046875, -0.0927734375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, r4.xw, -v1, -c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (42) mad_pp r0, c4.wzx, -c4.x, v0.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 4.052734375, -2.478515625, -2.9697265625, -3.3916015625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, c4.wzx, -c4.x, v0.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (43) mad_pp r0, -c4.yzwy, -t1.y, c4.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.7783203125, -2.126953125, -0.798828125, 1.7216796875
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4.yzwy, -t1.y, c4.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (44) mad r0, t0, r5.y, -c6.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.9404296875, -0.7158203125, 1.466796875, 1.794921875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0, r5.y, -c6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (45) mad_pp r0, -v0.x, -c5.xw, -v0";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.73779296875, 0.83837890625, 0.47900390625, 0.41650390625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -v0.x, -c5.xw, -v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (46) mad_pp r0, r4.zxyw, -c5, t2.x";
	PASSBEGIN
	TESTBEGIN
	def c1, -4.630859375, -1.4404296875, -2.0791015625, -4.060546875
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, r4.zxyw, -c5, t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (47) mad r0, c4.zxyw, -v1.yzwy, -v1.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.1552734375, -0.42919921875, 0.60400390625, -0.7568359375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad r0, c4.zxyw, -v1.yzwy, -v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (48) mad_pp r0, t0.wzx, -t0.zxyw, -c6.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 4.52734375, -3.947265625, -0.7822265625, 5.037109375
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, t0.wzx, -t0.zxyw, -c6.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (49) mad r0, v0, v0, v0.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.95336914063, 1.14086914063, 1.6181640625, 1.7275390625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, v0, v0, v0.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (50) mad r0, -t0.x, r5.x, t0.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -4.4755859375, -5.1943359375, -1.6943359375, -4.4755859375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -t0.x, r5.x, t0.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (51) mad r0, t0.y, -t0, t0.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.8759765625, -2.6259765625, 0.177734375, 0.787109375
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0.y, -t0, t0.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (52) mad r0, -t0.zxyw, r5.zxyw, c6.y";
	PASSBEGIN
	TESTBEGIN
	def c1, -2.259765625, -4.9755859375, -1.3779296875, -3.904296875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -t0.zxyw, r5.zxyw, c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (53) mad_pp r0, r4.xw, -r5.x, -r6.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.0380859375, 4.697265625, 4.697265625, 4.697265625
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, r4.xw, -r5.x, -r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (54) mad_pp r0, v0.xw, c5.y, -r6.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.71728515625, 1.2099609375, 1.2099609375, 1.2099609375
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, v0.xw, c5.y, -r6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (55) mad_pp r0, c4.xw, c4.xw, -v0.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.8916015625, 2.30078125, 2.30078125, 2.30078125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, c4.xw, c4.xw, -v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (56) mad r0, r4.wzx, -v1.wzx, c6.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.5927734375, -1.2294921875, 0.28466796875, 0.62841796875
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad r0, r4.wzx, -v1.wzx, c6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (57) mad r0, t0.yzwy, -t0.x, c6.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.5322265625, -4.947265625, 1.943359375, -3.5322265625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0.yzwy, -t0.x, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (58) mad_pp r0, c4, -t1.x, -c4.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.5380859375, -0.0380859375, -4.818359375, 4.197265625
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, c4, -t1.x, -c4.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (59) mad_pp r0, -v0.x, v0.x, -v0";
	PASSBEGIN
	TESTBEGIN
	def c1, -1.95336914063, -1.45336914063, -1.81274414063, -1.87524414063
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -v0.x, v0.x, -v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (60) mad_pp r0, r4.y, -c5.xw, t2";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.3505859375, -0.1669921875, 1.9267578125, 0.1767578125
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, r4.y, -c5.xw, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (61) mad_pp r0, -v0.zxyw, c5.wzx, v0.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.1494140625, 1.36767578125, 1.76904296875, 2.5205078125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -v0.zxyw, c5.wzx, v0.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (62) mad_pp r0, t0.zxyw, -t0.zxyw, -t0";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.87890625, -2.9072265625, 0.7490234375, -5.09765625
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, t0.zxyw, -t0.zxyw, -t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (63) mad_pp r0, -c4.xw, -v1.x, v1.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.86279296875, 2.5048828125, 2.5048828125, 2.5048828125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4.xw, -v1.x, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (64) mad r0, t0.xw, r5.zxyw, t0.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.0458984375, -4.197265625, -0.798828125, 1.466796875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0.xw, r5.zxyw, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (65) mad r0, -r4.x, -r5.zxyw, -r6.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.5478515625, 4.6318359375, 0.9404296875, -1.5205078125
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -r4.x, -r5.zxyw, -r6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (66) mad_pp r0, -r4.x, v1.xw, -c6.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.87841796875, 0.69775390625, 2.94775390625, 2.94775390625
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4.x, v1.xw, -c6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (67) mad r0, v0.zxyw, -v0.zxyw, v0.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.1943359375, -0.125244140625, 0.749755859375, 0.1630859375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, v0.zxyw, -v0.zxyw, v0.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (68) mad r0, -v0.xw, -c5.yzwy, -v0.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.75146484375, -1.4091796875, 0.7333984375, -0.8212890625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, -v0.xw, -c5.yzwy, -v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (69) mad r0, -t0.wzx, -r5.wzx, -c6.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 4.060546875, 2.416015625, 5.1318359375, 5.1318359375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -t0.wzx, -r5.wzx, -c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (70) mad r0, -r4.wzx, -c5.x, -t2.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.759765625, 4.474609375, 4.9755859375, 4.9755859375
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -r4.wzx, -c5.x, -t2.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (71) mad_pp r0, -c4.wzx, v1.y, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.7568359375, -0.1552734375, -0.79833984375, -0.79833984375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4.wzx, v1.y, -v1.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (72) mad_pp r0, t0.wzx, -r5.xw, -t0.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 4.916015625, 4.236328125, 3.6142578125, 0.8330078125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, t0.wzx, -r5.xw, -t0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (73) mad_pp r0, -v0.yzwy, -c5.zxyw, -r6";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.39794921875, -0.0341796875, -0.5400390625, 1.77587890625
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -v0.yzwy, -c5.zxyw, -r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (74) mad r0, -c4.yzwy, -c4.wzx, v0.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.271484375, 3.33203125, -3.083984375, 2.3916015625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, -c4.yzwy, -c4.wzx, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (75) mad_pp r0, -t0, -t0.y, t0.x";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.0615234375, -1.0302734375, -0.333984375, -3.724609375
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -t0, -t0.y, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (76) mad_pp r0, -c4.x, t1.xw, -c4.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.5380859375, 4.6142578125, 4.6142578125, 4.6142578125
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4.x, t1.xw, -c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (77) mad r0, t0.y, t0.y, c6.y";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.5302734375, -0.5302734375, -0.5302734375, -0.5302734375
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0.y, t0.y, c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (78) mad r0, -r4.wzx, r5.y, r6.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.638671875, 0.783203125, -0.9404296875, -1.2841796875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -r4.wzx, r5.y, r6.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (79) mad_pp r0, -c4, c4, -v0.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -4.7822265625, -1.7822265625, -3.83203125, -4.26953125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4, c4, -v0.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (80) mad r0, -r4, -c5.wzx, -t2.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.5205078125, -0.1708984375, 4.630859375, -1.603515625
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -r4, -c5.wzx, -t2.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (81) mad r0, -c4, -v1.xw, v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.70654296875, -0.41650390625, -0.4619140625, 1.2646484375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad r0, -c4, -v1.xw, v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (82) mad r0, t0.x, r5.yzwy, -c6.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.2841796875, 0.2978515625, -1.5205078125, 1.2841796875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0.x, r5.yzwy, -c6.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (83) mad r0, v0.y, -c5.yzwy, -r6.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.07958984375, -0.39794921875, 0.97412109375, 1.57958984375
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad r0, v0.y, -c5.yzwy, -r6.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (84) mad_pp r0, -c4.y, t1.yzwy, c4.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.8779296875, 1.3583984375, 3.1142578125, 1.9033203125
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4.y, t1.yzwy, c4.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (85) mad r0, -v0.y, c5, v0.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.76904296875, 0.79833984375, 1.21142578125, 0.33349609375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, -v0.y, c5, v0.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (86) mad r0, v0.y, -v0.y, -v0.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, -1.07836914063, -1.21899414063, -0.718994140625, -1.14086914063
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, v0.y, -v0.y, -v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (87) mad r0, t0.xw, t0.yzwy, -c6.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 3.5322265625, -1.93359375, 4.41015625, -0.630859375
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0.xw, t0.yzwy, -c6.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (88) mad r0, r4.yzwy, -v1.yzwy, c6.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.42333984375, 1.0205078125, -2.3427734375, -1.42333984375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad r0, r4.yzwy, -v1.yzwy, c6.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (89) mad r0, t0.yzwy, -r5.wzx, -t0.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 3.2705078125, -2.603515625, 1.416015625, -3.5380859375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0.yzwy, -r5.wzx, -t0.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (90) mad r0, -t0.yzwy, t0.wzx, t0.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.068359375, -4.81640625, 2.599609375, -0.0947265625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -t0.yzwy, t0.wzx, t0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (91) mad r0, c4.wzx, -t1.xw, c4";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.259765625, 1.298828125, 0.9580078125, 4.4580078125
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	mad r0, c4.wzx, -t1.xw, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * sat * pp
TESTCASE "mad - sat (0) mad r0, r4, r5, r6";
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

TESTCASE "mad - sat (1) mad_pp r0, v0, c5, r6";
	PASSBEGIN
	TESTBEGIN
	def c1, -3.37841796875, -1.42333984375, 0.2294921875, 0.0927734375
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (2) mad_sat r0, r4, v1, c6";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.2294921875, 0.0927734375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (3) mad_sat_pp r0, c4, c4, v0";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (4) mad r0, t0, r5, c6";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8818359375, -1.5595703125, 1.416015625, 1.310546875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (5) mad r0, c4, t1, c4";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.5380859375, -1.0595703125, -0.896484375, 4.248046875
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	mad r0, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (6) mad r0, r4, c5, t2";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8818359375, -1.5595703125, 1.416015625, 1.310546875
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad r0, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (7) mad r0, v0, c5, v0";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.76904296875, 0.52978515625, 0.4482421875, 2.1240234375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (8) mad r0, t0, t0, c6";
	PASSBEGIN
	TESTBEGIN
	def c1, 2.2509765625, -0.5302734375, 3.47265625, 2.16015625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (9) mad r0, c4, v1, v1";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.86279296875, 0.00146484375, -0.1611328125, 1.8896484375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad r0, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (10) mad r0, t0, r5, t0";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.5380859375, -1.0595703125, -0.896484375, 4.248046875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (11) mad r0, v0, v0, v0";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.95336914063, 0.718994140625, 1.5556640625, 1.7275390625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (12) mad r0, t0, t0, t0";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.9072265625, -0.0302734375, 1.16015625, 5.09765625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (13) mad_sat r0, v0, c5, r6";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.2294921875, 0.0927734375
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (14) mad r0, c4, c4, v0";
	PASSBEGIN
	TESTBEGIN
	def c1, 4.8603515625, 1.4228515625, 3.69140625, 4.19140625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (15) mad_sat_pp r0, r4, r5, r6";
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

TESTCASE "mad - sat (16) mad_pp r0, r4, v1, c6";
	PASSBEGIN
	TESTBEGIN
	def c1, -3.37841796875, -1.51416015625, 0.2294921875, 0.0927734375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (17) mad_sat_pp r0, t0, r5, c6";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (18) mad_sat_pp r0, c4, t1, c4";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 1.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (19) mad_sat_pp r0, r4, c5, t2";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (20) mad_sat_pp r0, v0, c5, v0";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.52978515625, 0.4482421875, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (21) mad_sat_pp r0, t0, t0, c6";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (22) mad_sat_pp r0, c4, v1, v1";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.00146484375, 0.0, 1.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (23) mad_sat_pp r0, t0, r5, t0";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 1.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (24) mad_sat_pp r0, v0, v0, v0";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.718994140625, 1.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - sat (25) mad_sat_pp r0, t0, t0, t0";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// masks & predication
TESTCASE "mad - Pred/Mask (0) mad r0, r4, r5, r6";
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

TESTCASE "mad - Pred/Mask (1) (!p0.x) mad_pp r0.x, v0, c5, r6";
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
	(!p0.x) mad_pp r0.x, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (2) (!p0.y) mad_sat r0.y, r4, v1, c6";
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
	(!p0.y) mad_sat r0.y, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (3) (p0.z) mad_sat_pp r0.z, c4, c4, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) mad_sat_pp r0.z, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (4) (p0.w) mad r0.w, t0, r5, c6";
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
	(p0.w) mad r0.w, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (5) (p0) mad_sat r0.xz, c4, t1, c4";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) mad_sat r0.xz, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (6) mad_sat r0.yw, r4, c5, t2";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.yw, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (7) (p0.y) mad r0.xyw, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -0.76904296875, 0.52978515625, 0.0, 2.1240234375
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) mad r0.xyw, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (8) (p0.z) mad r0.xyzw, t0, t0, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) mad r0.xyzw, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (9) (!p0.w) mad_sat_pp r0, c4, v1, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.00146484375, 0.0, 1.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) mad_sat_pp r0, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (10) mad_sat_pp r0.x, t0, r5, t0";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.x, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (11) mad_pp r0.y, v0, v0, v0";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.718994140625, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.y, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (12) mad r0.z, t0, t0, t0";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.16015625, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0.z, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (13) (!p0.x) mad_sat_pp r0.w, r4, r5, r6";
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
	(!p0.x) mad_sat_pp r0.w, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (14) (p0.w) mad_sat r0.xz, v0, c5, r6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.2294921875, 0.0
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) mad_sat r0.xz, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (15) (p0) mad_pp r0.yw, r4, v1, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, -1.51416015625, 0.0, 0.0927734375
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0) mad_pp r0.yw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (16) mad r0.xyw, c4, c4, v0";
	PASSBEGIN
	TESTBEGIN
	def c1, 4.8603515625, 1.4228515625, 0.0, 4.19140625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0.xyw, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (17) (!p0.y) mad_sat_pp r0.xyzw, t0, r5, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(!p0.y) mad_sat_pp r0.xyzw, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (18) (!p0.z) mad_pp r0.xz, r4, c5, t2";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.8818359375, 0.0, 1.416015625, 0.0
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) mad_pp r0.xz, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (19) (!p0.w) mad_pp r0.x, c4, t1, c4";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) mad_pp r0.x, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (20) (!p0) mad_sat_pp r0.xyw, t0, t0, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 1.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) mad_sat_pp r0.xyw, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (21) (!p0.x) mad_sat_pp r0.yw, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.52978515625, 0.0, 1.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) mad_sat_pp r0.yw, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (22) (p0.x) mad r0, t0, r5, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.5380859375, -1.0595703125, -0.896484375, 4.248046875
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) mad r0, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (23) (p0.y) mad r0.x, c4, v1, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -0.86279296875, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) mad r0.x, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (24) (p0) mad_sat_pp r0.y, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) mad_sat_pp r0.y, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (25) (!p0) mad_sat r0.z, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) mad_sat r0.z, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (26) mad_sat r0.w, v0, c5, r6";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0927734375
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.w, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (27) (!p0.w) mad_sat_pp r0.xyzw, r4, r5, r6";
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
	(!p0.w) mad_sat_pp r0.xyzw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (28) (p0.y) mad_sat r0.yw, c4, c4, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) mad_sat r0.yw, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (29) (p0.z) mad_pp r0, r4, v1, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -3.37841796875, -1.51416015625, 0.2294921875, 0.0927734375
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) mad_pp r0, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (30) mad r0.y, t0, r5, c6";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.5595703125, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0.y, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (31) (!p0.x) mad_pp r0.z, c4, t1, c4";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, -0.896484375, 0.0
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) mad_pp r0.z, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (32) (p0) mad r0.w, r4, c5, t2";
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
	(p0) mad r0.w, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (33) mad_sat r0.xz, v0, c5, v0";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.4482421875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.xz, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (34) (p0.x) mad_pp r0.xyw, c4, v1, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -0.86279296875, 0.00146484375, 0.0, 1.8896484375
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) mad_pp r0.xyw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (35) (p0.y) mad_sat_pp r0.xyzw, t0, r5, t0";
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
	(p0.y) mad_sat_pp r0.xyzw, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (36) (p0.w) mad r0.y, t0, t0, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) mad r0.y, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (37) (p0.x) mad r0.xz, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.95336914063, 0.0, 1.5556640625, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) mad r0.xz, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (38) (p0.z) mad_sat r0.yw, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) mad_sat r0.yw, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (39) (!p0) mad_sat r0.xyzw, v0, c5, r6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.2294921875, 0.0
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0) mad_sat r0.xyzw, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (40) (p0) mad_pp r0, c4, c4, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 1.4228515625, 0.0, 4.19140625
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) mad_pp r0, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (41) (!p0.z) mad_pp r0.x, r4, r5, r6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.8818359375, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) mad_pp r0.x, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (42) mad_sat r0.z, r4, v1, c6";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.2294921875, 0.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.z, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (43) (p0.z) mad_pp r0.w, c4, t1, c4";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 4.248046875
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) mad_pp r0.w, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (44) (p0) mad r0.xz, t0, r5, c6";
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
	(p0) mad r0.xz, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (45) (!p0.y) mad_pp r0.xyw, r4, c5, t2";
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
	(!p0.y) mad_pp r0.xyw, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (46) (!p0) mad_pp r0.xyzw, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.52978515625, 0.0, 2.1240234375
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) mad_pp r0.xyzw, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (47) mad_pp r0.xyzw, c4, v1, v1";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.86279296875, 0.00146484375, -0.1611328125, 1.8896484375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.xyzw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (48) mad_sat r0, t0, t0, c6";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (49) (p0.z) mad_sat_pp r0.y, t0, r5, t0";
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
	(p0.z) mad_sat_pp r0.y, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (50) (!p0.x) mad_sat_pp r0.xz, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) mad_sat_pp r0.xz, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (51) (p0.w) mad r0.yw, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.718994140625, 0.0, 1.7275390625
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) mad r0.yw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (52) (p0.y) mad_pp r0.y, r4, r5, r6";
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
	(p0.y) mad_pp r0.y, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (53) (p0.z) mad_pp r0.y, v0, c5, r6";
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
	(p0.z) mad_pp r0.y, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (54) (!p0.z) mad_sat_pp r0.z, t0, r5, c6";
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
	(!p0.z) mad_sat_pp r0.z, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (55) (!p0.w) mad r0.z, r4, c5, t2";
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
	(!p0.w) mad r0.z, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (56) (p0.z) mad_sat r0.z, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) mad_sat r0.z, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (57) (!p0.x) mad_sat_pp r0.z, t0, t0, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) mad_sat_pp r0.z, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (58) (p0.z) mad_sat_pp r0.z, c4, v1, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) mad_sat_pp r0.z, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (59) (!p0.w) mad_sat_pp r0.z, t0, r5, t0";
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
	(!p0.w) mad_sat_pp r0.z, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (60) (!p0.x) mad_pp r0.xz, r4, v1, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -3.37841796875, 0.0, 0.2294921875, 0.0
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) mad_pp r0.xz, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (61) (p0.x) mad r0.xz, c4, c4, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) mad r0.xz, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (62) mad_sat r0.xyw, c4, t1, c4";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 1.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.xyw, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (63) (!p0.z) mad_sat r0.xyw, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.0, 0.718994140625, 0.0, 1.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) mad_sat r0.xyw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (64) (p0.x) mad r0.xyzw, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) mad r0.xyzw, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (65) (!p0.y) mad_sat r0, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.0, 0.718994140625, 1.0, 1.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.y) mad_sat r0, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (66) (!p0.w) mad_sat_pp r0, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) mad_sat_pp r0, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (67) (p0.z) mad_pp r0.z, r4, r5, r6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.416015625, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) mad_pp r0.z, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (68) (!p0.y) mad r0.xz, r4, r5, r6";
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
	(!p0.y) mad r0.xz, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (69) (p0.y) mad_sat_pp r0, v0, c5, r6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.2294921875, 0.0927734375
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) mad_sat_pp r0, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (70) (!p0) mad_sat_pp r0, t0, r5, c6";
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
	(!p0) mad_sat_pp r0, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (71) (!p0.w) mad_pp r0, c4, t1, c4";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.5380859375, -1.0595703125, -0.896484375, 4.248046875
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) mad_pp r0, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (72) mad_sat r0, r4, c5, t2";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (73) (!p0.y) mad_sat r0, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.y) mad_sat r0, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (74) (!p0.x) mad_pp r0.x, r4, v1, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, -3.37841796875, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) mad_pp r0.x, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (75) (p0.z) mad_pp r0.x, c4, c4, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 4.8603515625, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) mad_pp r0.x, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (76) (!p0.w) mad r0.x, t0, r5, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.8818359375, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) mad r0.x, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (77) (p0) mad_sat_pp r0.x, r4, c5, t2";
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
	(p0) mad_sat_pp r0.x, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (78) (!p0.x) mad_sat r0.x, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) mad_sat r0.x, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (79) (p0.z) mad r0.x, t0, t0, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) mad r0.x, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (80) (!p0) mad_sat r0.x, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) mad_sat r0.x, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (81) (!p0.w) mad_pp r0.x, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) mad_pp r0.x, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (82) (!p0.z) mad_pp r0.y, c4, c4, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 1.4228515625, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) mad_pp r0.y, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (83) mad_sat r0.y, c4, t1, c4";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.y, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (84) (!p0) mad_pp r0.y, r4, c5, t2";
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
	(!p0) mad_pp r0.y, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (85) (p0.y) mad r0.y, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.52978515625, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) mad r0.y, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (86) (p0.z) mad_pp r0.y, c4, v1, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) mad_pp r0.y, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (87) (p0.x) mad_sat_pp r0.z, v0, c5, r6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.2294921875, 0.0
	dcl v0
	SET_PRED
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) mad_sat_pp r0.z, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (88) (p0.w) mad_sat r0.w, r4, v1, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0927734375
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) mad_sat r0.w, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (89) mad_sat_pp r0.w, c4, c4, v0";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.w, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (90) (!p0) mad_sat_pp r0.w, v0, c5, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) mad_sat_pp r0.w, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (91) (!p0) mad_sat r0.w, t0, t0, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) mad_sat r0.w, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (92) mad_sat r0.w, c4, v1, v1";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.w, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (93) (p0.y) mad_pp r0.w, t0, r5, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 4.248046875
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) mad_pp r0.w, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (94) (!p0.w) mad_sat r0.w, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) mad_sat r0.w, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (95) (!p0.z) mad_sat r0.w, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) mad_sat r0.w, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (96) (!p0.x) mad_sat r0.xz, t0, t0, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) mad_sat r0.xz, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (97) (p0) mad_sat_pp r0.xz, c4, v1, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) mad_sat_pp r0.xz, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (98) mad_sat_pp r0.xz, t0, r5, t0";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.xz, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (99) (!p0.z) mad r0.yw, r4, r5, r6";
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
	(!p0.z) mad r0.yw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (100) (!p0) mad r0.yw, v0, c5, r6";
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
	(!p0) mad r0.yw, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (101) (p0.z) mad_sat r0.yw, t0, r5, c6";
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
	(p0.z) mad_sat r0.yw, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (102) (!p0) mad_pp r0.yw, c4, t1, c4";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, -1.0595703125, 0.0, 4.248046875
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) mad_pp r0.yw, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (103) mad r0.yw, t0, t0, c6";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.5302734375, 0.0, 2.16015625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0.yw, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (104) (!p0.y) mad_sat r0.yw, c4, v1, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.00146484375, 0.0, 1.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.y) mad_sat r0.yw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (105) (!p0) mad_pp r0.yw, t0, r5, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, -1.0595703125, 0.0, 4.248046875
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	(!p0) mad_pp r0.yw, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (106) (!p0.z) mad_sat_pp r0.xyw, r4, r5, r6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 1.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) mad_sat_pp r0.xyw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (107) (p0.x) mad_pp r0.xyw, v0, c5, r6";
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
	(p0.x) mad_pp r0.xyw, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (108) (p0.x) mad_pp r0.xyw, r4, v1, c6";
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
	(p0.x) mad_pp r0.xyw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (109) (!p0) mad_sat_pp r0.xyw, t0, r5, c6";
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
	(!p0) mad_sat_pp r0.xyw, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (110) mad_sat_pp r0.xyw, t0, r5, t0";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 1.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.xyw, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (111) (!p0.w) mad_sat_pp r0.xyw, t0, t0, t0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 1.0
	dcl t0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) mad_sat_pp r0.xyw, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (112) (p0.w) mad r0.xyzw, r4, v1, c6";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -3.37841796875, -1.51416015625, 0.2294921875, 0.0927734375
	dcl v1
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) mad r0.xyzw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (113) (p0.y) mad r0.xyzw, c4, c4, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) mad r0.xyzw, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (114) (!p0.x) mad_pp r0.xyzw, c4, t1, c4";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) mad_pp r0.xyzw, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (115) (!p0) mad r0.xyzw, r4, c5, t2";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, -1.5595703125, 0.0, 1.310546875
	dcl t2
	SET_PRED
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	(!p0) mad r0.xyzw, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (116) (p0.x) mad_sat_pp r0.xyzw, v0, v0, v0";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) mad_sat_pp r0.xyzw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  235 tests generated.