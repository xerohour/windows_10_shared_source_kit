// Tests for Instruction: dpN
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
TESTCASE "dpN - Standard (0) dp3 r0.xyz, r4, r5";
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

TESTCASE "dpN - Standard (1) dp3_pp r0.xyz, -c4.x, v1.x";
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

TESTCASE "dpN - Standard (2) dp3 r0.xyz, -v0.y, -c5.y";
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

TESTCASE "dpN - Standard (3) dp3_pp r0.xyz, t0.zxyw, -r5.zxyw";
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

TESTCASE "dpN - Standard (4) dp3_pp r0.xyz, -r4.xw, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 4.4765625, 4.4765625, 4.4765625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -r4.xw, -r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (5) dp3 r0.xyz, c4.wzx, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.71044921875, -1.71044921875, -1.71044921875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, c4.wzx, -v1.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (6) dp3_pp r0.xyz, v0.yzwy, c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.30517578125, 0.30517578125, 0.30517578125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, v0.yzwy, c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (7) dp3_pp r0.xyz, -t0, -r5.x";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.623046875, -0.623046875, -0.623046875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -t0, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (8) dp3_pp r0.xyz, -t0.x, -r5";
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

TESTCASE "dpN - Standard (9) dp3 r0.xyz, -t0.y, r5.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, -1.001953125, -1.001953125, -1.001953125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -t0.y, r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (10) dp3 r0.xyz, -r4.zxyw, r5.y";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.2265625, -0.2265625, -0.2265625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -r4.zxyw, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (11) dp3 r0.xyz, v0.xw, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.02197265625, -0.02197265625, -0.02197265625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, v0.xw, c5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (12) dp3 r0.xyz, v0.wzx, c5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.78125, 1.78125, 1.78125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, v0.wzx, c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (13) dp3 r0.xyz, -r4.x, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0146484375, 0.0146484375, 0.0146484375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -r4.x, -r5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (14) dp3 r0.xyz, -r4.yzwy, -r5.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.052734375, 1.052734375, 1.052734375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -r4.yzwy, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (15) dp3_pp r0.xyz, r4.y, r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.6142578125, -1.6142578125, -1.6142578125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, r4.y, r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (16) dp3_pp r0.xyz, t0.wzx, -r5.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.7109375, 0.7109375, 0.7109375, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, t0.wzx, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (17) dp3 r0.xyz, c4, -v1.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.6787109375, 0.6787109375, 0.6787109375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, c4, -v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (18) dp3_pp r0.xyz, v0.x, -c5.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.1318359375, 0.1318359375, 0.1318359375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, v0.x, -c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (19) dp3 r0.xyz, -c4.zxyw, -v1";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.4873046875, -0.4873046875, -0.4873046875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, -c4.zxyw, -v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (20) dp3 r0.xyz, v0.xw, -c5.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.5146484375, -1.5146484375, -1.5146484375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, v0.xw, -c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (21) dp3_pp r0.xyz, -c4.xw, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.12548828125, 2.12548828125, 2.12548828125, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -c4.xw, -v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (22) dp3_pp r0.xyz, c4.yzwy, -v1.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.40625, -1.40625, -1.40625, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, c4.yzwy, -v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (23) dp3 r0.xyz, -t0, r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.1337890625, -2.1337890625, -2.1337890625, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -t0, r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (24) dp3 r0.xyz, -r4.wzx, -r5";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.486328125, 1.486328125, 1.486328125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -r4.wzx, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (25) dp3_pp r0.xyz, v0, -c5.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.44921875, 0.44921875, 0.44921875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, v0, -c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (26) dp3 r0.xyz, v0.y, -c5";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.5009765625, 0.5009765625, 0.5009765625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, v0.y, -c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (27) dp3 r0.xyz, -t0.y, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -5.56640625, -5.56640625, -5.56640625, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -t0.y, -r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (28) dp3 r0.xyz, c4.y, v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.19873046875, -2.19873046875, -2.19873046875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, c4.y, v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (29) dp3 r0.xyz, -v0.zxyw, c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.68310546875, -0.68310546875, -0.68310546875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, -v0.zxyw, c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (30) dp3 r0.xyz, -c4.xw, -v1.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.43359375, 0.43359375, 0.43359375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, -c4.xw, -v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (31) dp3_pp r0.xyz, -r4.xw, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.330078125, -1.330078125, -1.330078125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -r4.xw, r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (32) dp3 r0.xyz, t0, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.5361328125, -3.5361328125, -3.5361328125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, t0, -r5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (33) dp3_pp r0.xyz, -t0.xw, -r5";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.783203125, -2.783203125, -2.783203125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -t0.xw, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (34) dp3_pp r0.xyz, -t0.yzwy, -r5";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.529296875, -3.529296875, -3.529296875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -t0.yzwy, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (35) dp3 r0.xyz, -v0, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.0654296875, 2.0654296875, 2.0654296875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, -v0, -c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (36) dp3 r0.xyz, -r4.x, -r5.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.3515625, 0.3515625, 0.3515625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -r4.x, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (37) dp3 r0.xyz, -c4.x, -v1.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.732421875, -0.732421875, -0.732421875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, -c4.x, -v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (38) dp3_pp r0.xyz, -t0.x, r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.4248046875, 0.4248046875, 0.4248046875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -t0.x, r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (39) dp3 r0.xyz, t0.y, -r5.x";
	PASSBEGIN
	TESTBEGIN
	def c1, 3.673828125, 3.673828125, 3.673828125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, t0.y, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (40) dp3_pp r0.xyz, -r4.zxyw, -r5.x";
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

TESTCASE "dpN - Standard (41) dp3 r0.xyz, -t0.zxyw, r5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.818359375, 1.818359375, 1.818359375, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -t0.zxyw, r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (42) dp3 r0.xyz, -t0.zxyw, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.8818359375, 0.8818359375, 0.8818359375, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -t0.zxyw, -r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (43) dp3 r0.xyz, -r4.wzx, r5.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.955078125, -1.955078125, -1.955078125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -r4.wzx, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (44) dp3 r0.xyz, r4.wzx, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.927734375, -0.927734375, -0.927734375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, r4.wzx, r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (45) dp3_pp r0.xyz, c4.wzx, v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.63232421875, 0.63232421875, 0.63232421875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, c4.wzx, v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (46) dp3 r0.xyz, -c4.yzwy, v1.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.19140625, -0.19140625, -0.19140625, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, -c4.yzwy, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (47) dp3_pp r0.xyz, t0.yzwy, -r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.212890625, -2.212890625, -2.212890625, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, t0.yzwy, -r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (48) dp3 r0.xyz, -v0.yzwy, -c5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.09033203125, 1.09033203125, 1.09033203125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, -v0.yzwy, -c5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (49) dp4 r0.xyz, r4, r5";
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

TESTCASE "dpN - Standard (50) dp4_pp r0.xyz, -c4.x, v1.x";
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

TESTCASE "dpN - Standard (51) dp4 r0.xyz, -v0.y, -c5.y";
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

TESTCASE "dpN - Standard (52) dp4_pp r0.xyz, t0.zxyw, -r5.zxyw";
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

TESTCASE "dpN - Standard (53) dp4_pp r0.xyz, -r4.xw, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 6.8759765625, 6.8759765625, 6.8759765625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -r4.xw, -r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (54) dp4 r0.xyz, c4.wzx, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.54931640625, -1.54931640625, -1.54931640625, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, c4.wzx, -v1.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (55) dp4_pp r0.xyz, v0.yzwy, c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.08251953125, 0.08251953125, 0.08251953125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, v0.yzwy, c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (56) dp4_pp r0.xyz, -t0, -r5.x";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.73046875, 0.73046875, 0.73046875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -t0, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (57) dp4_pp r0.xyz, -t0.x, -r5";
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

TESTCASE "dpN - Standard (58) dp4 r0.xyz, -t0.y, r5.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1689453125, 1.1689453125, 1.1689453125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -t0.y, r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (59) dp4 r0.xyz, -r4.zxyw, r5.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.265625, 0.265625, 0.265625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -r4.zxyw, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (60) dp4 r0.xyz, v0.xw, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.65478515625, 0.65478515625, 0.65478515625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, v0.xw, c5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (61) dp4 r0.xyz, v0.wzx, c5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.06689453125, 2.06689453125, 2.06689453125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, v0.wzx, c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (62) dp4 r0.xyz, -r4.x, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.1318359375, 0.1318359375, 0.1318359375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -r4.x, -r5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (63) dp4 r0.xyz, -r4.yzwy, -r5.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.171875, -0.171875, -0.171875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -r4.yzwy, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (64) dp4_pp r0.xyz, r4.y, r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.8388671875, -2.8388671875, -2.8388671875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, r4.y, r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (65) dp4_pp r0.xyz, t0.wzx, -r5.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.59375, 0.59375, 0.59375, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, t0.wzx, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (66) dp4 r0.xyz, c4, -v1.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.52099609375, -0.52099609375, -0.52099609375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, c4, -v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (67) dp4_pp r0.xyz, v0.x, -c5.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.15380859375, -0.15380859375, -0.15380859375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, v0.x, -c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (68) dp4 r0.xyz, -c4.zxyw, -v1";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.71240234375, 0.71240234375, 0.71240234375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, -c4.zxyw, -v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (69) dp4 r0.xyz, v0.xw, -c5.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.19140625, -2.19140625, -2.19140625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, v0.xw, -c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (70) dp4_pp r0.xyz, -c4.xw, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.37158203125, 2.37158203125, 2.37158203125, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -c4.xw, -v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (71) dp4_pp r0.xyz, c4.yzwy, -v1.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.32080078125, -0.32080078125, -0.32080078125, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, c4.yzwy, -v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (72) dp4 r0.xyz, -t0, r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.4873046875, -3.4873046875, -3.4873046875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -t0, r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (73) dp4 r0.xyz, -r4.wzx, -r5";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.9150390625, 0.9150390625, 0.9150390625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -r4.wzx, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (74) dp4_pp r0.xyz, v0, -c5.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.6953125, 0.6953125, 0.6953125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, v0, -c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (75) dp4 r0.xyz, v0.y, -c5";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.58447265625, -0.58447265625, -0.58447265625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, v0.y, -c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (76) dp4 r0.xyz, -t0.y, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -7.7373046875, -7.7373046875, -7.7373046875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -t0.y, -r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (77) dp4 r0.xyz, c4.y, v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.42138671875, -2.42138671875, -2.42138671875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, c4.y, v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (78) dp4 r0.xyz, -v0.zxyw, c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.43701171875, -0.43701171875, -0.43701171875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, -v0.zxyw, c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (79) dp4 r0.xyz, -c4.xw, -v1.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.6796875, 0.6796875, 0.6796875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, -c4.xw, -v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (80) dp4_pp r0.xyz, -r4.xw, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.7294921875, -3.7294921875, -3.7294921875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -r4.xw, r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (81) dp4 r0.xyz, t0, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.0439453125, -3.0439453125, -3.0439453125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, t0, -r5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (82) dp4_pp r0.xyz, -t0.xw, -r5";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.3837890625, -0.3837890625, -0.3837890625, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -t0.xw, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (83) dp4_pp r0.xyz, -t0.yzwy, -r5";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -5.7001953125, -5.7001953125, -5.7001953125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -t0.yzwy, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (84) dp4 r0.xyz, -v0, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 3.26513671875, 3.26513671875, 3.26513671875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, -v0, -c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (85) dp4 r0.xyz, -r4.x, -r5.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.46875, 0.46875, 0.46875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -r4.x, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (86) dp4 r0.xyz, -c4.x, -v1.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.01806640625, -1.01806640625, -1.01806640625, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, -c4.x, -v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (87) dp4_pp r0.xyz, -t0.x, r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.7470703125, 0.7470703125, 0.7470703125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -t0.x, r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (88) dp4 r0.xyz, t0.y, -r5.x";
	PASSBEGIN
	TESTBEGIN
	def c1, 4.8984375, 4.8984375, 4.8984375, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, t0.y, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (89) dp4_pp r0.xyz, -r4.zxyw, -r5.x";
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

TESTCASE "dpN - Standard (90) dp4 r0.xyz, -t0.zxyw, r5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.5810546875, -0.5810546875, -0.5810546875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -t0.zxyw, r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (91) dp4 r0.xyz, -t0.zxyw, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.2353515625, 2.2353515625, 2.2353515625, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -t0.zxyw, -r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (92) dp4 r0.xyz, -r4.wzx, r5.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.6328125, -1.6328125, -1.6328125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -r4.wzx, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (93) dp4 r0.xyz, r4.wzx, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.4990234375, -1.4990234375, -1.4990234375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, r4.wzx, r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (94) dp4_pp r0.xyz, c4.wzx, v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.57373046875, 0.57373046875, 0.57373046875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, c4.wzx, v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (95) dp4 r0.xyz, -c4.yzwy, v1.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.03125, 0.03125, 0.03125, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, -c4.yzwy, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (96) dp4_pp r0.xyz, t0.yzwy, -r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.0419921875, -0.0419921875, -0.0419921875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, t0.yzwy, -r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Standard (97) dp4 r0.xyz, -v0.yzwy, -c5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.70263671875, 1.70263671875, 1.70263671875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, -v0.yzwy, -c5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * sat * pp
TESTCASE "DpN - Sat (0) dp3 r0.xyz, r4, r5";
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

TESTCASE "DpN - Sat (1) dp3_pp r0.xyz, -c4.x, -v1.x";
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

TESTCASE "DpN - Sat (2) dp3_sat r0.xyz, -v0.y, c5.y";
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

TESTCASE "DpN - Sat (3) dp3_sat_pp r0.xyz, t0.zxyw, -r5.zxyw";
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

TESTCASE "DpN - Sat (4) dp3_sat_pp r0.xyz, -r4.xw, r5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -r4.xw, r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (5) dp3_sat r0.xyz, c4.wzx, v1.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat r0.xyz, c4.wzx, v1.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (6) dp3 r0.xyz, v0.yzwy, -c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.30517578125, -0.30517578125, -0.30517578125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, v0.yzwy, -c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (7) dp3_sat_pp r0.xyz, -t0, -r5.x";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -t0, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (8) dp3_sat_pp r0.xyz, -t0.x, -r5";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.263671875, 0.263671875, 0.263671875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -t0.x, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (9) dp3 r0.xyz, -t0.y, r5.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, -1.001953125, -1.001953125, -1.001953125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -t0.y, r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (10) dp3 r0.xyz, -r4.zxyw, r5.y";
	PASSBEGIN
	TESTBEGIN
	def c1, -0.2265625, -0.2265625, -0.2265625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -r4.zxyw, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (11) dp3 r0.xyz, v0.xw, -c5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.02197265625, 0.02197265625, 0.02197265625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, v0.xw, -c5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (12) dp3 r0.xyz, v0.wzx, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.78125, -1.78125, -1.78125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, v0.wzx, -c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (13) dp3_sat_pp r0.xyz, -r4.x, r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -r4.x, r5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (14) dp3_sat_pp r0.xyz, -r4.yzwy, r5.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -r4.yzwy, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (15) dp3_pp r0.xyz, r4.y, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.6142578125, 1.6142578125, 1.6142578125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, r4.y, -r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (16) dp3_pp r0.xyz, t0.wzx, -r5.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.7109375, 0.7109375, 0.7109375, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, t0.wzx, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (17) dp3_sat r0.xyz, v0.x, -c5.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.1318359375, 0.1318359375, 0.1318359375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat r0.xyz, v0.x, -c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (18) dp3 r0.xyz, v0.zxyw, c5.x";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.2353515625, 1.2353515625, 1.2353515625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, v0.zxyw, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (19) dp3_sat r0.xyz, c4, v1.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat r0.xyz, c4, v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (20) dp3_sat r0.xyz, c4.xw, v1";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat r0.xyz, c4.xw, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (21) dp3_pp r0.xyz, -c4.wzx, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.63232421875, 0.63232421875, 0.63232421875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -c4.wzx, -v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (22) dp3_sat_pp r0.xyz, -c4.yzwy, -v1.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -c4.yzwy, -v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (23) dp3 r0.xyz, -t0, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.1337890625, 2.1337890625, 2.1337890625, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -t0, -r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (24) dp3_sat_pp r0.xyz, v0, -c5.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.44921875, 0.44921875, 0.44921875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, v0, -c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (25) dp3_sat r0.xyz, -c4.y, v1";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat r0.xyz, -c4.y, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (26) dp3_sat_pp r0.xyz, v0.zxyw, -c5";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.4873046875, 0.4873046875, 0.4873046875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, v0.zxyw, -c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (27) dp3 r0.xyz, -t0.zxyw, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.818359375, -1.818359375, -1.818359375, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3 r0.xyz, -t0.zxyw, -r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (28) dp3 r0.xyz, c4.zxyw, v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.15185546875, -1.15185546875, -1.15185546875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, c4.zxyw, v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (29) dp3_sat_pp r0.xyz, -t0.xw, -r5.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -t0.xw, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (30) dp3_pp r0.xyz, -r4.wzx, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.927734375, 0.927734375, 0.927734375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -r4.wzx, r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (31) dp3 r0.xyz, c4.yzwy, -v1.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.19140625, -0.19140625, -0.19140625, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, c4.yzwy, -v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (32) dp3_pp r0.xyz, -t0, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 3.5361328125, 3.5361328125, 3.5361328125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -t0, -r5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (33) dp3_pp r0.xyz, -t0.yzwy, -r5";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -3.529296875, -3.529296875, -3.529296875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -t0.yzwy, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (34) dp3_sat_pp r0.xyz, -v0, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -v0, -c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (35) dp3_pp r0.xyz, v0.x, -c5.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.17578125, 0.17578125, 0.17578125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, v0.x, -c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (36) dp3_sat_pp r0.xyz, -v0.x, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.732421875, 0.732421875, 0.732421875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat_pp r0.xyz, -v0.x, -c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (37) dp3_sat r0.xyz, t0.x, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.4248046875, 0.4248046875, 0.4248046875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, t0.x, -r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (38) dp3_sat r0.xyz, -t0.y, -r5.x";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, -t0.y, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (39) dp3_pp r0.xyz, r4.y, r5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -5.56640625, -5.56640625, -5.56640625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, r4.y, r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (40) dp3 r0.xyz, v0.y, c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.02783203125, -0.02783203125, -0.02783203125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, v0.y, c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (41) dp3_pp r0.xyz, -r4.zxyw, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.8818359375, 0.8818359375, 0.8818359375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -r4.zxyw, -r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (42) dp3_pp r0.xyz, -r4.xw, -r5.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.384765625, 2.384765625, 2.384765625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -r4.xw, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (43) dp3_pp r0.xyz, -v0.xw, c5.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.1962890625, -0.1962890625, -0.1962890625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, -v0.xw, c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (44) dp3_sat r0.xyz, r4.xw, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, r4.xw, -r5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (45) dp3_pp r0.xyz, r4.wzx, -r5";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.486328125, -1.486328125, -1.486328125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp3_pp r0.xyz, r4.wzx, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (46) dp3_sat r0.xyz, -r4.wzx, r5.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp3_sat r0.xyz, -r4.wzx, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (47) dp3 r0.xyz, -v0.yzwy, c5.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.6748046875, 0.6748046875, 0.6748046875, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp3 r0.xyz, -v0.yzwy, c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (48) dp3_sat r0.xyz, c4.yzwy, v1.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.26318359375, 0.26318359375, 0.26318359375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp3_sat r0.xyz, c4.yzwy, v1.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (49) dp4 r0.xyz, r4, r5";
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

TESTCASE "DpN - Sat (50) dp4_pp r0.xyz, -c4.x, -v1.x";
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

TESTCASE "DpN - Sat (51) dp4_sat r0.xyz, -v0.y, c5.y";
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

TESTCASE "DpN - Sat (52) dp4_sat_pp r0.xyz, t0.zxyw, -r5.zxyw";
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

TESTCASE "DpN - Sat (53) dp4_sat_pp r0.xyz, -r4.xw, r5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -r4.xw, r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (54) dp4_sat r0.xyz, c4.wzx, v1.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat r0.xyz, c4.wzx, v1.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (55) dp4 r0.xyz, v0.yzwy, -c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.08251953125, -0.08251953125, -0.08251953125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, v0.yzwy, -c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (56) dp4_sat_pp r0.xyz, -t0, -r5.x";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.73046875, 0.73046875, 0.73046875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -t0, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (57) dp4_sat_pp r0.xyz, -t0.x, -r5";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -t0.x, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (58) dp4 r0.xyz, -t0.y, r5.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.1689453125, 1.1689453125, 1.1689453125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -t0.y, r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (59) dp4 r0.xyz, -r4.zxyw, r5.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.265625, 0.265625, 0.265625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -r4.zxyw, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (60) dp4 r0.xyz, v0.xw, -c5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.65478515625, -0.65478515625, -0.65478515625, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, v0.xw, -c5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (61) dp4 r0.xyz, v0.wzx, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -2.06689453125, -2.06689453125, -2.06689453125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, v0.wzx, -c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (62) dp4_sat_pp r0.xyz, -r4.x, r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -r4.x, r5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (63) dp4_sat_pp r0.xyz, -r4.yzwy, r5.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.171875, 0.171875, 0.171875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -r4.yzwy, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (64) dp4_pp r0.xyz, r4.y, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.8388671875, 2.8388671875, 2.8388671875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, r4.y, -r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (65) dp4_pp r0.xyz, t0.wzx, -r5.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.59375, 0.59375, 0.59375, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, t0.wzx, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (66) dp4_sat r0.xyz, v0.x, -c5.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat r0.xyz, v0.x, -c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (67) dp4 r0.xyz, v0.zxyw, c5.x";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.912109375, 1.912109375, 1.912109375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, v0.zxyw, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (68) dp4_sat r0.xyz, c4, v1.zxyw";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.52099609375, 0.52099609375, 0.52099609375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat r0.xyz, c4, v1.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (69) dp4_sat r0.xyz, c4.xw, v1";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat r0.xyz, c4.xw, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (70) dp4_pp r0.xyz, -c4.wzx, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.57373046875, 0.57373046875, 0.57373046875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -c4.wzx, -v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (71) dp4_sat_pp r0.xyz, -c4.yzwy, -v1.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.32080078125, 0.32080078125, 0.32080078125, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -c4.yzwy, -v1.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (72) dp4 r0.xyz, -t0, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 3.4873046875, 3.4873046875, 3.4873046875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -t0, -r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (73) dp4_sat_pp r0.xyz, v0, -c5.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.6953125, 0.6953125, 0.6953125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, v0, -c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (74) dp4_sat r0.xyz, -c4.y, v1";
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat r0.xyz, -c4.y, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (75) dp4_sat_pp r0.xyz, v0.zxyw, -c5";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, v0.zxyw, -c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (76) dp4 r0.xyz, -t0.zxyw, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.5810546875, 0.5810546875, 0.5810546875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4 r0.xyz, -t0.zxyw, -r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (77) dp4 r0.xyz, c4.zxyw, v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.90576171875, -0.90576171875, -0.90576171875, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, c4.zxyw, v1.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (78) dp4_sat_pp r0.xyz, -t0.xw, -r5.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -t0.xw, -r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (79) dp4_pp r0.xyz, -r4.wzx, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.4990234375, 1.4990234375, 1.4990234375, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -r4.wzx, r5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (80) dp4 r0.xyz, c4.yzwy, -v1.y";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.03125, 0.03125, 0.03125, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, c4.yzwy, -v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (81) dp4_pp r0.xyz, -t0, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 3.0439453125, 3.0439453125, 3.0439453125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -t0, -r5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (82) dp4_pp r0.xyz, -t0.yzwy, -r5";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -5.7001953125, -5.7001953125, -5.7001953125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -t0.yzwy, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (83) dp4_sat_pp r0.xyz, -v0, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -v0, -c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (84) dp4_pp r0.xyz, v0.x, -c5.y";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.234375, 0.234375, 0.234375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, v0.x, -c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (85) dp4_sat_pp r0.xyz, -v0.x, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat_pp r0.xyz, -v0.x, -c5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (86) dp4_sat r0.xyz, t0.x, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.7470703125, 0.7470703125, 0.7470703125, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, t0.x, -r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (87) dp4_sat r0.xyz, -t0.y, -r5.x";
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, -t0.y, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (88) dp4_pp r0.xyz, r4.y, r5.xw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -7.7373046875, -7.7373046875, -7.7373046875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, r4.y, r5.xw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (89) dp4 r0.xyz, v0.y, c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.25048828125, -0.25048828125, -0.25048828125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, v0.y, c5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (90) dp4_pp r0.xyz, -r4.zxyw, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 2.2353515625, 2.2353515625, 2.2353515625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -r4.zxyw, -r5.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (91) dp4_pp r0.xyz, -r4.xw, -r5.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 3.73828125, 3.73828125, 3.73828125, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -r4.xw, -r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (92) dp4_pp r0.xyz, -v0.xw, c5.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -1.39599609375, -1.39599609375, -1.39599609375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, -v0.xw, c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (93) dp4_sat r0.xyz, r4.xw, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, r4.xw, -r5.yzwy
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (94) dp4_pp r0.xyz, r4.wzx, -r5";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.9150390625, -0.9150390625, -0.9150390625, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	dp4_pp r0.xyz, r4.wzx, -r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (95) dp4_sat r0.xyz, -r4.wzx, r5.x";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	dp4_sat r0.xyz, -r4.wzx, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (96) dp4 r0.xyz, -v0.yzwy, c5.zxyw";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, -0.41064453125, -0.41064453125, -0.41064453125, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	dp4 r0.xyz, -v0.yzwy, c5.zxyw
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "DpN - Sat (97) dp4_sat r0.xyz, c4.yzwy, v1.wzx";
	USEARBSWZ
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	dp4_sat r0.xyz, c4.yzwy, v1.wzx
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// masks & predication
TESTCASE "dpN - Pred/Mask (0) dp3 r0, r4, r5";
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

TESTCASE "dpN - Pred/Mask (1) (p0.x) dp3 r0.x, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.2880859375, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) dp3 r0.x, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (2) (p0.y) dp3 r0.y, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, -0.7333984375, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) dp3 r0.y, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (3) (p0.z) dp3 r0.z, t0, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, -1.220703125, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp3 r0.z, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (4) (!p0.w) dp3 r0.w, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -1.220703125
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp3 r0.w, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (5) dp3 r0.xz, v0, c5";
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

TESTCASE "dpN - Pred/Mask (6) (!p0) dp3 r0.yw, t0, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, -1.220703125
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp3 r0.yw, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (7) (!p0.z) dp3 r0.xyw, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.2880859375, 0.2880859375, 0.0, 0.2880859375
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) dp3 r0.xyw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (8) (!p0.y) dp3 r0.xyzw, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) dp3 r0.xyzw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (9) (!p0.w) dp3 r0, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.2880859375, 0.2880859375, 0.2880859375, 0.2880859375
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) dp3 r0, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (10) (!p0) dp3 r0.x, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -0.7333984375, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) dp3 r0.x, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (11) dp3 r0.y, t0, r5";
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

TESTCASE "dpN - Pred/Mask (12) (!p0.y) dp3 r0.z, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
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

TESTCASE "dpN - Pred/Mask (13) (p0.x) dp3 r0.w, t0, r5";
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
	(p0.x) dp3 r0.w, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (14) (p0.w) dp3 r0.xz, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.2880859375, 0.0, 0.2880859375, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) dp3 r0.xz, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (15) (p0.y) dp3 r0.yw, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) dp3 r0.yw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (16) dp3 r0.xyw, t0, r5";
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

TESTCASE "dpN - Pred/Mask (17) (p0.z) dp3 r0.xyzw, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -0.7333984375, -0.7333984375, -0.7333984375, -0.7333984375
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) dp3 r0.xyzw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (18) (p0.x) dp3 r0, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) dp3 r0, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (19) (!p0.w) dp3 r0.x, r4, r5";
	USEPREDICATE
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

TESTCASE "dpN - Pred/Mask (20) (p0) dp3 r0.y, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.2880859375, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) dp3 r0.y, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (21) (!p0.x) dp3 r0.z, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.2880859375, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) dp3 r0.z, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (22) dp3 r0.w, c4, v1";
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

TESTCASE "dpN - Pred/Mask (23) (p0.x) dp3 r0.yw, r4, r5";
	USEPREDICATE
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

TESTCASE "dpN - Pred/Mask (24) (!p0.z) dp3 r0.xz, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, -1.220703125, 0.0, -1.220703125, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp3 r0.xz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (25) (p0.w) dp3 r0.xyw, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) dp3 r0.xyw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (26) (p0) dp3 r0.xyzw, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.2880859375, 0.0, 0.2880859375
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) dp3 r0.xyzw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (27) (!p0.y) dp3 r0, t0, r5";
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
	(!p0.y) dp3 r0, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (28) (p0.y) dp3 r0.x, t0, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, -1.220703125, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp3 r0.x, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (29) (p0) dp3 r0.w, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -0.7333984375
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) dp3 r0.w, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (30) (!p0.x) dp3 r0.y, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) dp3 r0.y, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (31) (p0) dp3 r0.xyw, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0) dp3 r0.xyw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (32) (p0.x) dp3 r0.xyzw, t0, r5";
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
	(p0.x) dp3 r0.xyzw, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (33) dp3 r0.z, v0, c5";
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

TESTCASE "dpN - Pred/Mask (34) dp3 r0.yw, v0, c5";
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

TESTCASE "dpN - Pred/Mask (35) (!p0) dp3 r0.xz, t0, r5";
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
	(!p0) dp3 r0.xz, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (36) dp3 r0.x, v0, c5";
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

TESTCASE "dpN - Pred/Mask (37) dp3 r0.xyzw, r4, r5";
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

TESTCASE "dpN - Pred/Mask (38) (p0.z) dp3 r0, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, -0.7333984375, -0.7333984375, -0.7333984375, -0.7333984375
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) dp3 r0, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (39) (p0.z) dp3 r0.y, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp3 r0.y, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (40) (p0.y) dp3 r0.w, r4, r5";
	USEPREDICATE
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

TESTCASE "dpN - Pred/Mask (41) (!p0.w) dp3 r0.z, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) dp3 r0.z, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (42) (p0.y) dp3 r0.xz, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.2880859375, 0.0, 0.2880859375, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) dp3 r0.xz, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (43) (p0.w) dp3 r0.yw, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, -1.220703125, 0.0, -1.220703125
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp3 r0.yw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (44) (!p0.x) dp3 r0.xyw, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) dp3 r0.xyw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (45) dp4 r0, r4, r5";
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

TESTCASE "dpN - Pred/Mask (46) (p0.x) dp4 r0.x, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.48779296875, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) dp4 r0.x, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (47) (p0.y) dp4 r0.y, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.46630859375, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) dp4 r0.y, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (48) (p0.z) dp4 r0.z, t0, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.1787109375, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp4 r0.z, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (49) (!p0.w) dp4 r0.w, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 1.1787109375
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) dp4 r0.w, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (50) dp4 r0.xz, v0, c5";
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

TESTCASE "dpN - Pred/Mask (51) (!p0) dp4 r0.yw, t0, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 1.1787109375
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0) dp4 r0.yw, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (52) (!p0.z) dp4 r0.xyw, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 1.48779296875, 1.48779296875, 0.0, 1.48779296875
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.z) dp4 r0.xyw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (53) (!p0.y) dp4 r0.xyzw, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) dp4 r0.xyzw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (54) (!p0.w) dp4 r0, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.48779296875, 1.48779296875, 1.48779296875, 1.48779296875
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) dp4 r0, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (55) (!p0) dp4 r0.x, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.46630859375, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0) dp4 r0.x, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (56) dp4 r0.y, t0, r5";
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

TESTCASE "dpN - Pred/Mask (57) (!p0.y) dp4 r0.z, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
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

TESTCASE "dpN - Pred/Mask (58) (p0.x) dp4 r0.w, t0, r5";
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
	(p0.x) dp4 r0.w, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (59) (p0.w) dp4 r0.xz, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.48779296875, 0.0, 1.48779296875, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) dp4 r0.xz, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (60) (p0.y) dp4 r0.yw, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) dp4 r0.yw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (61) dp4 r0.xyw, t0, r5";
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

TESTCASE "dpN - Pred/Mask (62) (p0.z) dp4 r0.xyzw, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.46630859375, 0.46630859375, 0.46630859375, 0.46630859375
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) dp4 r0.xyzw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (63) (p0.x) dp4 r0, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.x) dp4 r0, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (64) (!p0.w) dp4 r0.x, r4, r5";
	USEPREDICATE
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

TESTCASE "dpN - Pred/Mask (65) (p0) dp4 r0.y, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 1.48779296875, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) dp4 r0.y, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (66) (!p0.x) dp4 r0.z, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.48779296875, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) dp4 r0.z, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (67) dp4 r0.w, c4, v1";
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

TESTCASE "dpN - Pred/Mask (68) (p0.x) dp4 r0.yw, r4, r5";
	USEPREDICATE
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

TESTCASE "dpN - Pred/Mask (69) (!p0.z) dp4 r0.xz, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 1.1787109375, 0.0, 1.1787109375, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) dp4 r0.xz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (70) (p0.w) dp4 r0.xyw, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.w) dp4 r0.xyw, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (71) (p0) dp4 r0.xyzw, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 1.48779296875, 0.0, 1.48779296875
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) dp4 r0.xyzw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (72) (!p0.y) dp4 r0, t0, r5";
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
	(!p0.y) dp4 r0, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (73) (p0.y) dp4 r0.x, t0, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.1787109375, 0.0, 0.0, 0.0
	dcl t0
	SET_PRED
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) dp4 r0.x, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (74) (p0) dp4 r0.w, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.46630859375
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0) dp4 r0.w, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (75) (!p0.x) dp4 r0.y, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) dp4 r0.y, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (76) (p0) dp4 r0.xyw, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0) dp4 r0.xyw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (77) (p0.x) dp4 r0.xyzw, t0, r5";
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
	(p0.x) dp4 r0.xyzw, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (78) dp4 r0.z, v0, c5";
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

TESTCASE "dpN - Pred/Mask (79) dp4 r0.yw, v0, c5";
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

TESTCASE "dpN - Pred/Mask (80) (!p0) dp4 r0.xz, t0, r5";
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
	(!p0) dp4 r0.xz, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (81) dp4 r0.x, v0, c5";
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

TESTCASE "dpN - Pred/Mask (82) dp4 r0.xyzw, r4, r5";
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

TESTCASE "dpN - Pred/Mask (83) (p0.z) dp4 r0, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.46630859375, 0.46630859375, 0.46630859375, 0.46630859375
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.z) dp4 r0, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (84) (p0.z) dp4 r0.y, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) dp4 r0.y, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (85) (p0.y) dp4 r0.w, r4, r5";
	USEPREDICATE
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

TESTCASE "dpN - Pred/Mask (86) (!p0.w) dp4 r0.z, v0, c5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.w) dp4 r0.z, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (87) (p0.y) dp4 r0.xz, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 1.48779296875, 0.0, 1.48779296875, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(p0.y) dp4 r0.xz, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (88) (p0.w) dp4 r0.yw, r4, r5";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 1.1787109375, 0.0, 1.1787109375
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) dp4 r0.yw, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "dpN - Pred/Mask (89) (!p0.x) dp4 r0.xyw, c4, v1";
	USEPREDICATE
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.w
	(!p0.x) dp4 r0.xyw, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  286 tests generated.