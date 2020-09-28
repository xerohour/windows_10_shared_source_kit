// Tests for Instruction: mad
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

TESTCASE "mad - Standard (1) mad_pp r0, -v0.x, c5.x, -r6.x"
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

TESTCASE "mad - Standard (2) mad r0, r4.y, -v1.y, -c6.y"
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

TESTCASE "mad - Standard (3) mad_pp r0, -c4.zxyw, -c4.zxyw, v0.zxyw"
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

TESTCASE "mad - Standard (4) mad_pp r0, t0, -r5.x, c6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.9755859375, -3.1943359375, -4.474609375, 1.759765625
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, t0, -r5.x, c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (5) mad r0, c4.x, -t1, -c4.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.8193359375, 2.1533203125, 0.0458984375, 0.8330078125
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	mad r0, c4.x, -t1, -c4.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (6) mad r0, -r4.y, c5.zxyw, -t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1708984375, -0.2568359375, -0.5341796875, 2.4267578125
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -r4.y, c5.zxyw, -t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (7) mad r0, v0.zxyw, c5.y, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0634765625, 1.07666015625, 1.02978515625, 1.0693359375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, v0.zxyw, c5.y, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (8) mad_pp r0, -t0, -t0.y, -c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.5322265625, 2.4072265625, 1.009765625, -0.630859375
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -t0, -t0.y, -c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (9) mad_pp r0, -c4.y, -v1, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.02783203125, 0.84521484375, 0.66357421875, 0.23974609375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4.y, -v1, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (10) mad r0, t0.x, r5.zxyw, t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.0458984375, 2.5380859375, -1.1533203125, -3.6142578125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0.x, r5.zxyw, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (11) mad r0, v0.zxyw, v0.x, -v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.15380859375, 0.484619140625, -0.366943359375, -0.01416015625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, v0.zxyw, v0.x, -v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (12) mad r0, t0, t0.zxyw, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.353515625, -0.0615234375, -0.333984375, 1.31640625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0, t0.zxyw, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (13) mad_pp r0, -r4.x, -r5.y, -r6.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.8095703125, 1.4404296875, 1.2841796875, 0.9404296875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4.x, -r5.y, -r6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (14) mad r0, v0.y, -c5, r6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.60595703125, -1.51416015625, -1.24169921875, -2.11962890625
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad r0, v0.y, -c5, r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (15) mad r0, c4.y, c4.x, -v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.4228515625, 1.4228515625, 1.4228515625, 1.4228515625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, c4.y, c4.x, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (16) mad_pp r0, -r4.zxyw, v1, c6.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.1279296875, -1.53271484375, -1.24169921875, -2.3427734375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4.zxyw, v1, c6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (17) mad_pp r0, -c4, t1.x, c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.4755859375, -2.6943359375, -3.974609375, 2.259765625
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -c4, t1.x, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (18) mad_pp r0, r4, -c5.x, t2.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.8818359375, -3.3505859375, -4.474609375, 2.103515625
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, r4, -c5.x, t2.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (19) mad r0, -t0.x, r5, -c6"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.8818359375, 1.6533203125, -1.5478515625, 3.7705078125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -t0.x, r5, -c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (20) mad_pp r0, -v0.y, -c5, -v0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.70654296875, -0.93896484375, -0.71142578125, -0.25537109375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -v0.y, -c5, -v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (21) mad r0, t0.zxyw, t0, c6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.853515625, 0.4384765625, 0.166015625, 1.81640625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0.zxyw, t0, c6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (22) mad r0, c4, v1.x, -v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.64404296875, -0.90966796875, -1.7373046875, 0.9423828125
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad r0, c4, v1.x, -v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (23) mad_pp r0, -t0, -r5, -t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.4755859375, 0.8779296875, 2.478515625, 0.623046875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -t0, -r5, -t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (24) mad_pp r0, -v0, -v0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.95336914063, 1.21899414063, 1.6962890625, 1.8056640625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -v0, -v0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (25) mad_pp r0, -t0.x, -t0, -t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.8447265625, 2.8759765625, 5.009765625, -5.380859375
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -t0.x, -t0, -t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (26) mad r0, v0, c5.y, -r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.71728515625, 1.51416015625, -0.5458984375, 1.2099609375
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad r0, v0, c5.y, -r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (27) mad_pp r0, -r4, v1.x, -c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.37841796875, 2.33154296875, 0.8779296875, -0.4892578125
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4, v1.x, -c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (28) mad_pp r0, c4, -c4, -v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -4.8603515625, -1.4228515625, -3.69140625, -4.19140625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, c4, -c4, -v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (29) mad_pp r0, -v0, -c5.x, -v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.73779296875, -1.34716796875, -2.3466796875, -2.5205078125
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -v0, -c5.x, -v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (30) mad r0, r4.x, c5, -t2.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 5.1318359375, 1.4404296875, 2.5478515625, -1.0205078125
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad r0, r4.x, c5, -t2.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (31) mad r0, -t0.x, t0.x, -c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.2509765625, -2.2509765625, -2.2509765625, -2.2509765625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -t0.x, t0.x, -c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (32) mad_pp r0, c4.x, -v1.y, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.13916015625, 0.13916015625, 0.13916015625, 0.13916015625
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, c4.x, -v1.y, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (33) mad r0, -v0.x, v0.y, -v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.961181640625, -0.961181640625, -0.961181640625, -0.961181640625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, -v0.x, v0.y, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (34) mad r0, r4.y, -r5.x, r6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.3505859375, -3.3505859375, -3.3505859375, -3.3505859375
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0, r4.y, -r5.x, r6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (35) mad r0, -t0.y, -r5.y, c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.7158203125, -1.7158203125, -1.7158203125, -1.7158203125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -t0.y, -r5.y, c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (36) mad_pp r0, c4.y, t1.y, -c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8779296875, 0.8779296875, 1.5966796875, -1.9033203125
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, c4.y, t1.y, -c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (37) mad r0, -t0.y, -r5.x, -t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 3.6943359375, 3.6943359375, 3.6943359375, 3.6943359375
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -t0.y, -r5.x, -t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (38) mad_pp r0, -t0.y, -t0.x, -t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.8759765625, 2.8759765625, 2.8759765625, 2.8759765625
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -t0.y, -t0.x, -t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (39) mad r0, -r4.x, -v1.zxyw, -c6.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.16357421875, -0.12841796875, 1.53271484375, 0.30224609375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad r0, -r4.x, -v1.zxyw, -c6.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (40) mad r0, -c4.x, c4.y, -v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.8916015625, -2.8916015625, -2.8916015625, -2.8916015625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, -c4.x, c4.y, -v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (41) mad r0, -v0.x, c5.zxyw, -v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.02294921875, 1.26904296875, -0.57666015625, -1.80712890625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, -v0.x, c5.zxyw, -v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (42) mad_pp r0, t0.y, -t0.zxyw, -c6.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.259765625, -0.2822265625, 0.5302734375, 2.880859375
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, t0.y, -t0.zxyw, -c6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (43) mad r0, -v0.y, -v0.zxyw, v0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.25244140625, 1.46118164063, 0.718994140625, 1.34521484375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0, -v0.y, -v0.zxyw, v0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (44) mad_pp r0, -r4.zxyw, r5.zxyw, -r6.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.677734375, -2.0380859375, 1.5595703125, -0.966796875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -r4.zxyw, r5.zxyw, -r6.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (45) mad r0, v0.zxyw, -c5.zxyw, -r6.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.2294921875, 3.37841796875, 1.42333984375, -0.0927734375
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad r0, v0.zxyw, -c5.zxyw, -r6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (46) mad r0, -t0.zxyw, -r5.zxyw, -c6.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.166015625, 5.1318359375, 1.3779296875, 3.560546875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, -t0.zxyw, -r5.zxyw, -c6.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (47) mad r0, c4.zxyw, t1.zxyw, -c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.759765625, 5.4755859375, 1.8779296875, 4.404296875
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	mad r0, c4.zxyw, t1.zxyw, -c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (48) mad r0, r4.zxyw, c5.y, t2.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.626953125, -1.6533203125, -1.5595703125, -1.298828125
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad r0, r4.zxyw, c5.y, t2.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (49) mad_pp r0, c4.zxyw, v1.zxyw, v1.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.1611328125, -0.86279296875, 0.00146484375, 1.8896484375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, c4.zxyw, v1.zxyw, v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (50) mad r0, t0.zxyw, r5.y, -t0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.529296875, 1.7841796875, 0.8779296875, -1.642578125
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0, t0.zxyw, r5.y, -t0.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Standard (51) mad_pp r0, -t0.zxyw, -t0.y, t0.zxyw"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.052734375, -0.0615234375, -0.0302734375, 0.056640625
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, -t0.zxyw, -t0.y, t0.zxyw
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

TESTCASE "mad - sat (1) mad_pp r0, v0, c5, r6"
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

TESTCASE "mad - sat (2) mad_sat r0, r4, v1, c6"
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

TESTCASE "mad - sat (3) mad_sat_pp r0, c4, c4, v0"
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

TESTCASE "mad - sat (4) mad r0, t0, r5, c6"
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

TESTCASE "mad - sat (5) mad r0, c4, t1, c4"
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

TESTCASE "mad - sat (6) mad r0, r4, c5, t2"
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

TESTCASE "mad - sat (7) mad r0, v0, c5, v0"
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

TESTCASE "mad - sat (8) mad r0, t0, t0, c6"
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

TESTCASE "mad - sat (9) mad r0, c4, v1, v1"
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

TESTCASE "mad - sat (10) mad r0, t0, r5, t0"
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

TESTCASE "mad - sat (11) mad r0, v0, v0, v0"
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

TESTCASE "mad - sat (12) mad r0, t0, t0, t0"
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

TESTCASE "mad - sat (13) mad_sat r0, v0, c5, r6"
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

TESTCASE "mad - sat (14) mad r0, c4, c4, v0"
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

TESTCASE "mad - sat (15) mad_sat_pp r0, r4, r5, r6"
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

TESTCASE "mad - sat (16) mad_pp r0, r4, v1, c6"
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

TESTCASE "mad - sat (17) mad_sat_pp r0, t0, r5, c6"
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

TESTCASE "mad - sat (18) mad_sat_pp r0, c4, t1, c4"
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

TESTCASE "mad - sat (19) mad_sat_pp r0, r4, c5, t2"
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

TESTCASE "mad - sat (20) mad_sat_pp r0, v0, c5, v0"
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

TESTCASE "mad - sat (21) mad_sat_pp r0, t0, t0, c6"
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

TESTCASE "mad - sat (22) mad_sat_pp r0, c4, v1, v1"
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

TESTCASE "mad - sat (23) mad_sat_pp r0, t0, r5, t0"
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

TESTCASE "mad - sat (24) mad_sat_pp r0, v0, v0, v0"
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

TESTCASE "mad - sat (25) mad_sat_pp r0, t0, t0, t0"
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

TESTCASE "mad - Pred/Mask (1) mad_pp r0.x, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.37841796875, 0.0, 0.0, 0.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.x, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (2) mad_sat r0.y, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.y, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (3) mad_sat_pp r0.z, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.z, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (4) mad r0.w, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.310546875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0.w, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (5) mad r0.xz, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.5380859375, 0.0, -0.896484375, 0.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	mad r0.xz, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (6) mad r0.yw, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.5595703125, 0.0, 1.310546875
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad r0.yw, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (7) mad r0.xyw, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.76904296875, 0.52978515625, 0.0, 2.1240234375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0.xyw, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (8) mad r0.xyzw, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.2509765625, -0.5302734375, 3.47265625, 2.16015625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0.xyzw, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (9) mad_sat_pp r0, c4, v1, v1"
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

TESTCASE "mad - Pred/Mask (10) mad_sat r0.x, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.x, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (11) mad_pp r0.y, v0, v0, v0"
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

TESTCASE "mad - Pred/Mask (12) mad r0.z, t0, t0, t0"
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

TESTCASE "mad - Pred/Mask (13) mad_sat_pp r0.w, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.w, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (14) mad_sat r0.xz, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.2294921875, 0.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.xz, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (15) mad_pp r0.yw, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.51416015625, 0.0, 0.0927734375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.yw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (16) mad r0.xyw, c4, c4, v0"
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

TESTCASE "mad - Pred/Mask (17) mad_sat_pp r0.xyw, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 1.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.xyw, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (18) mad_sat_pp r0.xyzw, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 1.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.xyzw, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (19) mad_sat_pp r0.xz, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 0.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.xz, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (20) mad_sat_pp r0.yw, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.52978515625, 0.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.yw, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (21) mad_sat_pp r0, t0, t0, c6"
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

TESTCASE "mad - Pred/Mask (22) mad_pp r0, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.5380859375, -1.0595703125, -0.896484375, 4.248046875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (23) mad_sat r0, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.718994140625, 1.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (24) mad_sat_pp r0, t0, t0, t0"
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

TESTCASE "mad - Pred/Mask (25) mad r0.x, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.86279296875, 0.0, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad r0.x, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (26) mad_sat r0, v0, c5, r6"
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

TESTCASE "mad - Pred/Mask (27) mad_sat_pp r0, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.2294921875, 0.0927734375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (28) mad r0, c4, c4, v0"
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

TESTCASE "mad - Pred/Mask (29) mad_sat_pp r0, t0, r5, c6"
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

TESTCASE "mad - Pred/Mask (30) mad r0, c4, t1, c4"
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

TESTCASE "mad - Pred/Mask (31) mad r0, r4, c5, t2"
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

TESTCASE "mad - Pred/Mask (32) mad_sat r0, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.52978515625, 0.4482421875, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (33) mad r0.x, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8818359375, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad r0.x, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (34) mad_sat r0.x, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.x, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (35) mad_sat_pp r0.x, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.x, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (36) mad_sat r0.x, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.x, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (37) mad_sat_pp r0.x, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.x, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (38) mad_pp r0.x, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8818359375, 0.0, 0.0, 0.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.x, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (39) mad_pp r0.x, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.76904296875, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.x, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (40) mad_sat r0.x, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.x, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (41) mad_pp r0.x, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.95336914063, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.x, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (42) mad_pp r0.x, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.9072265625, 0.0, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.x, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (43) mad_sat_pp r0.y, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.y, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (44) mad_sat r0.y, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.y, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (45) mad_pp r0.y, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.4228515625, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.y, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (46) mad_sat_pp r0.y, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.y, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (47) mad r0.y, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.0595703125, 0.0, 0.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	mad r0.y, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (48) mad r0.y, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.5595703125, 0.0, 0.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad r0.y, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (49) mad_sat r0.y, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.52978515625, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.y, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (50) mad_sat_pp r0.y, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.y, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (51) mad_sat_pp r0.y, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.00146484375, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.y, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (52) mad_pp r0.y, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.0595703125, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.y, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (53) mad r0.y, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.0302734375, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0.y, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (54) mad_sat r0.z, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.z, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (55) mad_sat r0.z, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.2294921875, 0.0
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.z, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (56) mad_sat_pp r0.z, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.2294921875, 0.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.z, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (57) mad_sat r0.z, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.z, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (58) mad_sat_pp r0.z, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.z, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (59) mad_sat r0.z, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.z, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (60) mad_sat_pp r0.z, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.4482421875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.z, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (61) mad_sat_pp r0.z, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.z, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (62) mad_pp r0.z, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -0.1611328125, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.z, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (63) mad_sat_pp r0.z, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.z, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (64) mad_sat r0.z, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.z, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (65) mad_sat_pp r0.w, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0927734375
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.w, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (66) mad_sat_pp r0.w, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0927734375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.w, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (67) mad r0.w, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 4.19140625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0.w, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (68) mad_sat_pp r0.w, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.w, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (69) mad_pp r0.w, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.310546875
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.w, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (70) mad_sat_pp r0.w, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.w, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (71) mad_sat_pp r0.w, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.w, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (72) mad_pp r0.w, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.8896484375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.w, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (73) mad r0.w, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 4.248046875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0.w, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (74) mad r0.w, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.7275390625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0.w, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (75) mad r0.w, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 5.09765625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0.w, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (76) mad_pp r0.xz, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8818359375, 0.0, 1.416015625, 0.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.xz, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (77) mad_sat_pp r0.xz, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.2294921875, 0.0
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.xz, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (78) mad_pp r0.xz, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.8603515625, 0.0, 3.69140625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.xz, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (79) mad_sat r0.xz, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.xz, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (80) mad_sat_pp r0.xz, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.4482421875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.xz, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (81) mad r0.xz, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.2509765625, 0.0, 3.47265625, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0.xz, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (82) mad_sat_pp r0.xz, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.xz, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (83) mad_sat r0.xz, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.xz, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (84) mad r0.xz, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.95336914063, 0.0, 1.5556640625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0.xz, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (85) mad_sat r0.xz, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.xz, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (86) mad_sat r0.yw, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.yw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (87) mad_pp r0.yw, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.42333984375, 0.0, 0.0927734375
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.yw, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (88) mad_pp r0.yw, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.4228515625, 0.0, 4.19140625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.yw, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (89) mad r0.yw, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.5595703125, 0.0, 1.310546875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0.yw, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (90) mad_pp r0.yw, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.0595703125, 0.0, 4.248046875
	dcl t1
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.yw, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (91) mad_sat r0.yw, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.yw, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (92) mad_sat r0.yw, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.00146484375, 0.0, 1.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.yw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (93) mad_sat r0.yw, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.yw, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (94) mad r0.yw, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.718994140625, 0.0, 1.7275390625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0.yw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (95) mad r0.yw, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.0302734375, 0.0, 5.09765625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0.yw, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (96) mad_sat r0.xyw, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 1.0
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.xyw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (97) mad_sat r0.xyw, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0927734375
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.xyw, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (98) mad_sat r0.xyw, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0927734375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.xyw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (99) mad r0.xyw, c4, t1, c4"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.5380859375, -1.0595703125, 0.0, 4.248046875
	dcl t1
	mov r0, c0.y
	mov r2, c0.z
	mad r0.xyw, c4, t1, c4
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (100) mad_pp r0.xyw, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8818359375, -1.5595703125, 0.0, 1.310546875
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.xyw, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (101) mad_sat r0.xyw, t0, t0, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.xyw, t0, t0, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (102) mad r0.xyw, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.86279296875, 0.00146484375, 0.0, 1.8896484375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad r0.xyw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (103) mad_pp r0.xyw, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.5380859375, -1.0595703125, 0.0, 4.248046875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.xyw, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (104) mad_sat r0.xyw, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.718994140625, 0.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.xyw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (105) mad_sat_pp r0.xyw, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 1.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.xyw, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (106) mad_pp r0.xyzw, r4, r5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8818359375, -1.5595703125, 1.416015625, 1.310546875
	mov r4, c4
	mov r5, c5
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.xyzw, r4, r5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (107) mad_sat r0.xyzw, v0, c5, r6"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.2294921875, 0.0927734375
	dcl v0
	mov r6, c6
	mov r0, c0.y
	mov r2, c0.w
	mad_sat r0.xyzw, v0, c5, r6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (108) mad_pp r0.xyzw, r4, v1, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.37841796875, -1.51416015625, 0.2294921875, 0.0927734375
	dcl v1
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_pp r0.xyzw, r4, v1, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (109) mad r0.xyzw, c4, c4, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 4.8603515625, 1.4228515625, 3.69140625, 4.19140625
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0.xyzw, c4, c4, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (110) mad r0.xyzw, t0, r5, c6"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.8818359375, -1.5595703125, 1.416015625, 1.310546875
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad r0.xyzw, t0, r5, c6
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (111) mad_sat_pp r0.xyzw, r4, c5, t2"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 1.0
	dcl t2
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.xyzw, r4, c5, t2
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (112) mad r0.xyzw, v0, c5, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.76904296875, 0.52978515625, 0.4482421875, 2.1240234375
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad r0.xyzw, v0, c5, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (113) mad r0.xyzw, c4, v1, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.86279296875, 0.00146484375, -0.1611328125, 1.8896484375
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	mad r0.xyzw, c4, v1, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (114) mad_sat r0.xyzw, t0, r5, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 1.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	mad_sat r0.xyzw, t0, r5, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (115) mad_sat_pp r0.xyzw, v0, v0, v0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.718994140625, 1.0, 1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	mad_sat_pp r0.xyzw, v0, v0, v0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mad - Pred/Mask (116) mad r0.xyzw, t0, t0, t0"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.9072265625, -0.0302734375, 1.16015625, 5.09765625
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	mad r0.xyzw, t0, t0, t0
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  195 tests generated.