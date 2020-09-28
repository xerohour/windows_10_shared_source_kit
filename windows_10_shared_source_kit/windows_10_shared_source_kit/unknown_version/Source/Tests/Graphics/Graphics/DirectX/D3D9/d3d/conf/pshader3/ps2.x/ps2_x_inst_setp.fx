#include "include.fx"

#define PS_SETUP \
	ps_2_x \
	def c0, 1.0, 0.0, -1.0, 0.0

// following line sets p0 to 0000, mainly for write mask tests
// we can rely on this since setp_eq will be tested separately
#define CLEAR_P		setp_eq p0, c0.x, c0.y

// Test macros.  These will set red or blue to 1 if a component is
// set to the wrong value:
// red set:  if component should be 0 but is 1
#define X0 (p0.x) mov r2.r, c0.x
#define Y0 (p0.y) mov r2.r, c0.x
#define Z0 (p0.z) mov r2.r, c0.x
#define W0 (p0.w) mov r2.r, c0.x
// blue set: if component should be 1 but is 0
#define X1 (!p0.x) mov r2.b, c0.x
#define Y1 (!p0.y) mov r2.b, c0.x
#define Z1 (!p0.z) mov r2.b, c0.x
#define W1 (!p0.w) mov r2.b, c0.x

// The purpose of the two cmp ops is to remove green if either red or blue
// channels are set.  This makes it more obvious that something was wrong:
#define PS_END \
	mov r2.ga, c0.x \
	add_sat r2.g, r2.g, -r2.r \
	add_sat r2.g, r2.g, -r2.b \
	mov oC0, r2

#define SET_VS \
	VertexShaderConstant[0] = <mFinal>; \
	VertexShader = <NormalVS>;

#define SET_PS_CONSTANTS
#define USEARBSWZ bool UseArbSwizzle = true;
#define TESTCASE Technique < String Name =
#define PASSBEGIN \
	String Shape = "TinyQuad"; \
	bool UsePredicate = true; \
> { Pass P0 { 
#define TESTBEGIN PixelShader = asm { PS_SETUP
#define TESTEND PS_END }; SET_VS SET_PS_CONSTANTS } }

VertexShader NormalVS =
	asm
	{
		vs_2_0
		dcl_position	v0
		def c5, 1.25, 0.0, 2.0, -0.75
		def c6, -1.25, 0.0, 2.0, 1.0
		def c7, 0.625, 0.0, 1.0, 0.375
		def c8, 0.625, 0.0, 1.0, 0.5
		m4x4 oPos, v0, c0
		mov oT0, c5
		mov oT1, c6
		mov oD0, c7
		mov oD1, c8
	};


// Tests:

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// op * source registers * modifiers * swizzles
TESTCASE "setp - gt (0) setp_gt p0, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_gt p0, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - gt (1) setp_gt p0, -c4.x, v1.x";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_gt p0, -c4.x, v1.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - gt (2) setp_gt p0, v0.y, -c5.y";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_gt p0, v0.y, -c5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - gt (3) setp_gt p0, -t0.zxyw, -r5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_gt p0, -t0.zxyw, -r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp - gt (4) setp_gt p0, -r4.xw, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_gt p0, -r4.xw, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - gt (5) setp_gt p0, c4.wzx, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_gt p0, c4.wzx, -v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - gt (6) setp_gt p0, -v0.yzwy, c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_gt p0, -v0.yzwy, c5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - gt (7) setp_gt p0, -t0, -r5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_gt p0, -t0, -r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - gt (8) setp_gt p0, -t0.x, -r5";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_gt p0, -t0.x, -r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - gt (9) setp_gt p0, t0.y, r5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_gt p0, t0.y, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - gt (10) setp_gt p0, r4.zxyw, r5.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_gt p0, r4.zxyw, r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - gt (11) setp_gt p0, v0.xw, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_gt p0, v0.xw, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - gt (12) setp_gt p0, v0.wzx, c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_gt p0, v0.wzx, c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - gt (13) setp_gt p0, r4.x, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_gt p0, r4.x, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - gt (14) setp_gt p0, r4.yzwy, -r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_gt p0, r4.yzwy, -r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - gt (15) setp_gt p0, -r4.y, r5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_gt p0, -r4.y, r5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - gt (16) setp_gt p0, -t0.wzx, -r5.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_gt p0, -t0.wzx, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - gt (17) setp_gt p0, c4, v1.zxyw";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_gt p0, c4, v1.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W0
	TESTEND

TESTCASE "setp - gt (18) setp_gt p0, -c4.zxyw, -v1";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_gt p0, -c4.zxyw, -v1
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - gt (19) setp_gt p0, -r4.xw, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_gt p0, -r4.xw, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W0
	TESTEND

TESTCASE "setp - gt (20) setp_gt p0, -c4.xw, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_gt p0, -c4.xw, -v1.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - gt (21) setp_gt p0, c4.yzwy, -v1.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_gt p0, c4.yzwy, -v1.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - gt (22) setp_gt p0, -c4, -v1.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_gt p0, -c4, -v1.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp - gt (23) setp_gt p0, -v0, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_gt p0, -v0, -c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W1
	TESTEND

TESTCASE "setp - gt (24) setp_gt p0, v0.x, -c5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_gt p0, v0.x, -c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W1
	TESTEND

TESTCASE "setp - gt (25) setp_gt p0, c4.y, v1";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_gt p0, c4.y, v1
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - gt (26) setp_gt p0, v0.zxyw, -c5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_gt p0, v0.zxyw, -c5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - gt (27) setp_gt p0, -v0.xw, c5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_gt p0, -v0.xw, c5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - gt (28) setp_gt p0, -t0.wzx, r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_gt p0, -t0.wzx, r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - gt (29) setp_gt p0, t0.yzwy, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_gt p0, t0.yzwy, -r5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - gt (30) setp_gt p0, t0.x, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_gt p0, t0.x, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - gt (31) setp_gt p0, -t0.xw, r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_gt p0, -t0.xw, r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - gt (32) setp_gt p0, r4.wzx, r5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_gt p0, r4.wzx, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - gt (33) setp_gt p0, c4, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_gt p0, c4, -v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W0
	TESTEND

TESTCASE "setp - gt (34) setp_gt p0, t0, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_gt p0, t0, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W0
	TESTEND

TESTCASE "setp - gt (35) setp_gt p0, -r4.x, -r5.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_gt p0, -r4.x, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - gt (36) setp_gt p0, v0.x, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_gt p0, v0.x, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - gt (37) setp_gt p0, -v0.y, -c5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_gt p0, -v0.y, -c5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - gt (38) setp_gt p0, -t0.y, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_gt p0, -t0.y, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - gt (39) setp_gt p0, t0.y, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_gt p0, t0.y, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W0
	TESTEND

TESTCASE "setp - gt (40) setp_gt p0, -v0.zxyw, c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_gt p0, -v0.zxyw, c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - gt (41) setp_gt p0, -c4.zxyw, v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_gt p0, -c4.zxyw, v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp - gt (42) setp_gt p0, -c4.zxyw, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_gt p0, -c4.zxyw, -v1.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - gt (43) setp_gt p0, -c4.xw, -v1.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_gt p0, -c4.xw, -v1.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - gt (44) setp_gt p0, -r4.wzx, r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_gt p0, -r4.wzx, r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - gt (45) setp_gt p0, -v0.wzx, -c5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_gt p0, -v0.wzx, -c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W1
	TESTEND

TESTCASE "setp - gt (46) setp_gt p0, -r4.yzwy, -r5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_gt p0, -r4.yzwy, -r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - gt (47) setp_gt p0, r4.yzwy, -r5.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_gt p0, r4.yzwy, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - gt (48) setp_gt p0, -t0.yzwy, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_gt p0, -t0.yzwy, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - lt (0) setp_lt p0, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_lt p0, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp - lt (1) setp_lt p0, -c4.x, v1.x";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_lt p0, -c4.x, v1.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - lt (2) setp_lt p0, v0.y, -c5.y";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_lt p0, v0.y, -c5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - lt (3) setp_lt p0, -t0.zxyw, -r5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_lt p0, -t0.zxyw, -r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - lt (4) setp_lt p0, -r4.xw, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_lt p0, -r4.xw, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - lt (5) setp_lt p0, c4.wzx, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_lt p0, c4.wzx, -v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - lt (6) setp_lt p0, -v0.yzwy, c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_lt p0, -v0.yzwy, c5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W0
	TESTEND

TESTCASE "setp - lt (7) setp_lt p0, -t0, -r5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_lt p0, -t0, -r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - lt (8) setp_lt p0, -t0.x, -r5";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_lt p0, -t0.x, -r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - lt (9) setp_lt p0, t0.y, r5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_lt p0, t0.y, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W1
	TESTEND

TESTCASE "setp - lt (10) setp_lt p0, r4.zxyw, r5.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_lt p0, r4.zxyw, r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp - lt (11) setp_lt p0, v0.xw, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_lt p0, v0.xw, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - lt (12) setp_lt p0, v0.wzx, c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_lt p0, v0.wzx, c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - lt (13) setp_lt p0, r4.x, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_lt p0, r4.x, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - lt (14) setp_lt p0, r4.yzwy, -r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_lt p0, r4.yzwy, -r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W1
	TESTEND

TESTCASE "setp - lt (15) setp_lt p0, -r4.y, r5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_lt p0, -r4.y, r5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - lt (16) setp_lt p0, -t0.wzx, -r5.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_lt p0, -t0.wzx, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - lt (17) setp_lt p0, c4, v1.zxyw";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_lt p0, c4, v1.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W1
	TESTEND

TESTCASE "setp - lt (18) setp_lt p0, -c4.zxyw, -v1";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_lt p0, -c4.zxyw, -v1
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - lt (19) setp_lt p0, -r4.xw, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_lt p0, -r4.xw, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W1
	TESTEND

TESTCASE "setp - lt (20) setp_lt p0, -c4.xw, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_lt p0, -c4.xw, -v1.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - lt (21) setp_lt p0, c4.yzwy, -v1.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_lt p0, c4.yzwy, -v1.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - lt (22) setp_lt p0, -c4, -v1.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_lt p0, -c4, -v1.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W0
	TESTEND

TESTCASE "setp - lt (23) setp_lt p0, -v0, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_lt p0, -v0, -c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - lt (24) setp_lt p0, v0.x, -c5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_lt p0, v0.x, -c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - lt (25) setp_lt p0, c4.y, v1";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_lt p0, c4.y, v1
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W1
	TESTEND

TESTCASE "setp - lt (26) setp_lt p0, v0.zxyw, -c5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_lt p0, v0.zxyw, -c5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - lt (27) setp_lt p0, -v0.xw, c5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_lt p0, -v0.xw, c5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - lt (28) setp_lt p0, -t0.wzx, r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_lt p0, -t0.wzx, r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - lt (29) setp_lt p0, t0.yzwy, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_lt p0, t0.yzwy, -r5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - lt (30) setp_lt p0, t0.x, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_lt p0, t0.x, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - lt (31) setp_lt p0, -t0.xw, r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_lt p0, -t0.xw, r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - lt (32) setp_lt p0, r4.wzx, r5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_lt p0, r4.wzx, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - lt (33) setp_lt p0, c4, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_lt p0, c4, -v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp - lt (34) setp_lt p0, t0, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_lt p0, t0, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp - lt (35) setp_lt p0, -r4.x, -r5.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_lt p0, -r4.x, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - lt (36) setp_lt p0, v0.x, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_lt p0, v0.x, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - lt (37) setp_lt p0, -v0.y, -c5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_lt p0, -v0.y, -c5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - lt (38) setp_lt p0, -t0.y, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_lt p0, -t0.y, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - lt (39) setp_lt p0, t0.y, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_lt p0, t0.y, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - lt (40) setp_lt p0, -v0.zxyw, c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_lt p0, -v0.zxyw, c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - lt (41) setp_lt p0, -c4.zxyw, v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_lt p0, -c4.zxyw, v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W0
	TESTEND

TESTCASE "setp - lt (42) setp_lt p0, -c4.zxyw, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_lt p0, -c4.zxyw, -v1.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - lt (43) setp_lt p0, -c4.xw, -v1.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_lt p0, -c4.xw, -v1.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - lt (44) setp_lt p0, -r4.wzx, r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_lt p0, -r4.wzx, r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - lt (45) setp_lt p0, -v0.wzx, -c5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_lt p0, -v0.wzx, -c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W0
	TESTEND

TESTCASE "setp - lt (46) setp_lt p0, -r4.yzwy, -r5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_lt p0, -r4.yzwy, -r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - lt (47) setp_lt p0, r4.yzwy, -r5.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_lt p0, r4.yzwy, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - lt (48) setp_lt p0, -t0.yzwy, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_lt p0, -t0.yzwy, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - ge (0) setp_ge p0, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ge p0, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W0
	TESTEND

TESTCASE "setp - ge (1) setp_ge p0, -c4.x, v1.x";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ge p0, -c4.x, v1.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - ge (2) setp_ge p0, v0.y, -c5.y";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ge p0, v0.y, -c5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ge (3) setp_ge p0, -t0.zxyw, -r5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ge p0, -t0.zxyw, -r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W1
	TESTEND

TESTCASE "setp - ge (4) setp_ge p0, -r4.xw, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ge p0, -r4.xw, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ge (5) setp_ge p0, c4.wzx, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ge p0, c4.wzx, -v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ge (6) setp_ge p0, -v0.yzwy, c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ge p0, -v0.yzwy, c5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W1
	TESTEND

TESTCASE "setp - ge (7) setp_ge p0, -t0, -r5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ge p0, -t0, -r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - ge (8) setp_ge p0, -t0.x, -r5";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ge p0, -t0.x, -r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - ge (9) setp_ge p0, t0.y, r5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ge p0, t0.y, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W0
	TESTEND

TESTCASE "setp - ge (10) setp_ge p0, r4.zxyw, r5.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ge p0, r4.zxyw, r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W0
	TESTEND

TESTCASE "setp - ge (11) setp_ge p0, v0.xw, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ge p0, v0.xw, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - ge (12) setp_ge p0, v0.wzx, c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ge p0, v0.wzx, c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - ge (13) setp_ge p0, r4.x, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ge p0, r4.x, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ge (14) setp_ge p0, r4.yzwy, -r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ge p0, r4.yzwy, -r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - ge (15) setp_ge p0, -r4.y, r5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ge p0, -r4.y, r5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - ge (16) setp_ge p0, -t0.wzx, -r5.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ge p0, -t0.wzx, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - ge (17) setp_ge p0, c4, v1.zxyw";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ge p0, c4, v1.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W0
	TESTEND

TESTCASE "setp - ge (18) setp_ge p0, -c4.zxyw, -v1";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ge p0, -c4.zxyw, -v1
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - ge (19) setp_ge p0, -r4.xw, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ge p0, -r4.xw, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W0
	TESTEND

TESTCASE "setp - ge (20) setp_ge p0, -c4.xw, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ge p0, -c4.xw, -v1.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ge (21) setp_ge p0, c4.yzwy, -v1.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ge p0, c4.yzwy, -v1.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - ge (22) setp_ge p0, -c4, -v1.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ge p0, -c4, -v1.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W1
	TESTEND

TESTCASE "setp - ge (23) setp_ge p0, -v0, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ge p0, -v0, -c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ge (24) setp_ge p0, v0.x, -c5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ge p0, v0.x, -c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W1
	TESTEND

TESTCASE "setp - ge (25) setp_ge p0, c4.y, v1";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ge p0, c4.y, v1
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - ge (26) setp_ge p0, v0.zxyw, -c5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ge p0, v0.zxyw, -c5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - ge (27) setp_ge p0, -v0.xw, c5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ge p0, -v0.xw, c5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - ge (28) setp_ge p0, -t0.wzx, r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ge p0, -t0.wzx, r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - ge (29) setp_ge p0, t0.yzwy, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ge p0, t0.yzwy, -r5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - ge (30) setp_ge p0, t0.x, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ge p0, t0.x, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ge (31) setp_ge p0, -t0.xw, r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ge p0, -t0.xw, r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ge (32) setp_ge p0, r4.wzx, r5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ge p0, r4.wzx, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - ge (33) setp_ge p0, c4, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ge p0, c4, -v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W0
	TESTEND

TESTCASE "setp - ge (34) setp_ge p0, t0, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ge p0, t0, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W0
	TESTEND

TESTCASE "setp - ge (35) setp_ge p0, -r4.x, -r5.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ge p0, -r4.x, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - ge (36) setp_ge p0, v0.x, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ge p0, v0.x, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - ge (37) setp_ge p0, -v0.y, -c5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ge p0, -v0.y, -c5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - ge (38) setp_ge p0, -t0.y, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ge p0, -t0.y, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ge (39) setp_ge p0, t0.y, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ge p0, t0.y, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ge (40) setp_ge p0, -v0.zxyw, c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ge p0, -v0.zxyw, c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - ge (41) setp_ge p0, -c4.zxyw, v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ge p0, -c4.zxyw, v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp - ge (42) setp_ge p0, -c4.zxyw, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ge p0, -c4.zxyw, -v1.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - ge (43) setp_ge p0, -c4.xw, -v1.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ge p0, -c4.xw, -v1.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ge (44) setp_ge p0, -r4.wzx, r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ge p0, -r4.wzx, r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W1
	TESTEND

TESTCASE "setp - ge (45) setp_ge p0, -v0.wzx, -c5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ge p0, -v0.wzx, -c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W1
	TESTEND

TESTCASE "setp - ge (46) setp_ge p0, -r4.yzwy, -r5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ge p0, -r4.yzwy, -r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - ge (47) setp_ge p0, r4.yzwy, -r5.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ge p0, r4.yzwy, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - ge (48) setp_ge p0, -t0.yzwy, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ge p0, -t0.yzwy, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - le (0) setp_le p0, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_le p0, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - le (1) setp_le p0, -c4.x, v1.x";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_le p0, -c4.x, v1.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - le (2) setp_le p0, v0.y, -c5.y";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_le p0, v0.y, -c5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - le (3) setp_le p0, -t0.zxyw, -r5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_le p0, -t0.zxyw, -r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W0
	TESTEND

TESTCASE "setp - le (4) setp_le p0, -r4.xw, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_le p0, -r4.xw, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - le (5) setp_le p0, c4.wzx, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_le p0, c4.wzx, -v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - le (6) setp_le p0, -v0.yzwy, c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_le p0, -v0.yzwy, c5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - le (7) setp_le p0, -t0, -r5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_le p0, -t0, -r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - le (8) setp_le p0, -t0.x, -r5";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_le p0, -t0.x, -r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - le (9) setp_le p0, t0.y, r5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_le p0, t0.y, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W1
	TESTEND

TESTCASE "setp - le (10) setp_le p0, r4.zxyw, r5.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_le p0, r4.zxyw, r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - le (11) setp_le p0, v0.xw, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_le p0, v0.xw, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - le (12) setp_le p0, v0.wzx, c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_le p0, v0.wzx, c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - le (13) setp_le p0, r4.x, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_le p0, r4.x, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - le (14) setp_le p0, r4.yzwy, -r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_le p0, r4.yzwy, -r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W1
	TESTEND

TESTCASE "setp - le (15) setp_le p0, -r4.y, r5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_le p0, -r4.y, r5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - le (16) setp_le p0, -t0.wzx, -r5.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_le p0, -t0.wzx, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - le (17) setp_le p0, c4, v1.zxyw";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_le p0, c4, v1.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W1
	TESTEND

TESTCASE "setp - le (18) setp_le p0, -c4.zxyw, -v1";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_le p0, -c4.zxyw, -v1
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - le (19) setp_le p0, -r4.xw, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_le p0, -r4.xw, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W1
	TESTEND

TESTCASE "setp - le (20) setp_le p0, -c4.xw, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_le p0, -c4.xw, -v1.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - le (21) setp_le p0, c4.yzwy, -v1.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_le p0, c4.yzwy, -v1.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - le (22) setp_le p0, -c4, -v1.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_le p0, -c4, -v1.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W0
	TESTEND

TESTCASE "setp - le (23) setp_le p0, -v0, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_le p0, -v0, -c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W0
	TESTEND

TESTCASE "setp - le (24) setp_le p0, v0.x, -c5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_le p0, v0.x, -c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - le (25) setp_le p0, c4.y, v1";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_le p0, c4.y, v1
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - le (26) setp_le p0, v0.zxyw, -c5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_le p0, v0.zxyw, -c5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - le (27) setp_le p0, -v0.xw, c5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_le p0, -v0.xw, c5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - le (28) setp_le p0, -t0.wzx, r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_le p0, -t0.wzx, r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - le (29) setp_le p0, t0.yzwy, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_le p0, t0.yzwy, -r5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - le (30) setp_le p0, t0.x, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_le p0, t0.x, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - le (31) setp_le p0, -t0.xw, r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_le p0, -t0.xw, r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - le (32) setp_le p0, r4.wzx, r5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_le p0, r4.wzx, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - le (33) setp_le p0, c4, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_le p0, c4, -v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp - le (34) setp_le p0, t0, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_le p0, t0, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp - le (35) setp_le p0, -r4.x, -r5.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_le p0, -r4.x, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - le (36) setp_le p0, v0.x, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_le p0, v0.x, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - le (37) setp_le p0, -v0.y, -c5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_le p0, -v0.y, -c5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - le (38) setp_le p0, -t0.y, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_le p0, -t0.y, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - le (39) setp_le p0, t0.y, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_le p0, t0.y, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W1
	TESTEND

TESTCASE "setp - le (40) setp_le p0, -v0.zxyw, c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_le p0, -v0.zxyw, c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - le (41) setp_le p0, -c4.zxyw, v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_le p0, -c4.zxyw, v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W0
	TESTEND

TESTCASE "setp - le (42) setp_le p0, -c4.zxyw, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_le p0, -c4.zxyw, -v1.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - le (43) setp_le p0, -c4.xw, -v1.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_le p0, -c4.xw, -v1.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - le (44) setp_le p0, -r4.wzx, r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_le p0, -r4.wzx, r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - le (45) setp_le p0, -v0.wzx, -c5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_le p0, -v0.wzx, -c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W0
	TESTEND

TESTCASE "setp - le (46) setp_le p0, -r4.yzwy, -r5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_le p0, -r4.yzwy, -r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - le (47) setp_le p0, r4.yzwy, -r5.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_le p0, r4.yzwy, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W1
	TESTEND

TESTCASE "setp - le (48) setp_le p0, -t0.yzwy, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_le p0, -t0.yzwy, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - eq (0) setp_eq p0, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_eq p0, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W0
	TESTEND

TESTCASE "setp - eq (1) setp_eq p0, -c4.x, v1.x";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_eq p0, -c4.x, v1.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (2) setp_eq p0, v0.y, -c5.y";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_eq p0, v0.y, -c5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - eq (3) setp_eq p0, -t0.zxyw, -r5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_eq p0, -t0.zxyw, -r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W0
	TESTEND

TESTCASE "setp - eq (4) setp_eq p0, -r4.xw, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_eq p0, -r4.xw, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (5) setp_eq p0, c4.wzx, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_eq p0, c4.wzx, -v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (6) setp_eq p0, -v0.yzwy, c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_eq p0, -v0.yzwy, c5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W1
	TESTEND

TESTCASE "setp - eq (7) setp_eq p0, -t0, -r5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_eq p0, -t0, -r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (8) setp_eq p0, -t0.x, -r5";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_eq p0, -t0.x, -r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (9) setp_eq p0, t0.y, r5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_eq p0, t0.y, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - eq (10) setp_eq p0, r4.zxyw, r5.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_eq p0, r4.zxyw, r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - eq (11) setp_eq p0, v0.xw, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_eq p0, v0.xw, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (12) setp_eq p0, v0.wzx, c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_eq p0, v0.wzx, c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - eq (13) setp_eq p0, r4.x, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_eq p0, r4.x, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (14) setp_eq p0, r4.yzwy, -r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_eq p0, r4.yzwy, -r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (15) setp_eq p0, -r4.y, r5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_eq p0, -r4.y, r5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (16) setp_eq p0, -t0.wzx, -r5.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_eq p0, -t0.wzx, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (17) setp_eq p0, c4, v1.zxyw";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_eq p0, c4, v1.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (18) setp_eq p0, -c4.zxyw, -v1";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_eq p0, -c4.zxyw, -v1
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (19) setp_eq p0, -r4.xw, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_eq p0, -r4.xw, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (20) setp_eq p0, -c4.xw, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_eq p0, -c4.xw, -v1.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (21) setp_eq p0, c4.yzwy, -v1.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_eq p0, c4.yzwy, -v1.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (22) setp_eq p0, -c4, -v1.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_eq p0, -c4, -v1.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - eq (23) setp_eq p0, -v0, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_eq p0, -v0, -c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - eq (24) setp_eq p0, v0.x, -c5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_eq p0, v0.x, -c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (25) setp_eq p0, c4.y, v1";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_eq p0, c4.y, v1
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - eq (26) setp_eq p0, v0.zxyw, -c5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_eq p0, v0.zxyw, -c5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (27) setp_eq p0, -v0.xw, c5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_eq p0, -v0.xw, c5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (28) setp_eq p0, -t0.wzx, r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_eq p0, -t0.wzx, r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (29) setp_eq p0, t0.yzwy, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_eq p0, t0.yzwy, -r5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (30) setp_eq p0, t0.x, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_eq p0, t0.x, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (31) setp_eq p0, -t0.xw, r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_eq p0, -t0.xw, r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (32) setp_eq p0, r4.wzx, r5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_eq p0, r4.wzx, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (33) setp_eq p0, c4, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_eq p0, c4, -v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (34) setp_eq p0, t0, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_eq p0, t0, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (35) setp_eq p0, -r4.x, -r5.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_eq p0, -r4.x, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (36) setp_eq p0, v0.x, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_eq p0, v0.x, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (37) setp_eq p0, -v0.y, -c5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_eq p0, -v0.y, -c5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (38) setp_eq p0, -t0.y, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_eq p0, -t0.y, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (39) setp_eq p0, t0.y, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_eq p0, t0.y, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W1
	TESTEND

TESTCASE "setp - eq (40) setp_eq p0, -v0.zxyw, c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_eq p0, -v0.zxyw, c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (41) setp_eq p0, -c4.zxyw, v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_eq p0, -c4.zxyw, v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (42) setp_eq p0, -c4.zxyw, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_eq p0, -c4.zxyw, -v1.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (43) setp_eq p0, -c4.xw, -v1.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_eq p0, -c4.xw, -v1.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (44) setp_eq p0, -r4.wzx, r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_eq p0, -r4.wzx, r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - eq (45) setp_eq p0, -v0.wzx, -c5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_eq p0, -v0.wzx, -c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (46) setp_eq p0, -r4.yzwy, -r5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_eq p0, -r4.yzwy, -r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - eq (47) setp_eq p0, r4.yzwy, -r5.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_eq p0, r4.yzwy, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W1
	TESTEND

TESTCASE "setp - eq (48) setp_eq p0, -t0.yzwy, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_eq p0, -t0.yzwy, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - ne (0) setp_ne p0, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ne p0, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W1
	TESTEND

TESTCASE "setp - ne (1) setp_ne p0, -c4.x, v1.x";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ne p0, -c4.x, v1.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (2) setp_ne p0, v0.y, -c5.y";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ne p0, v0.y, -c5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - ne (3) setp_ne p0, -t0.zxyw, -r5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ne p0, -t0.zxyw, -r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W1
	TESTEND

TESTCASE "setp - ne (4) setp_ne p0, -r4.xw, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ne p0, -r4.xw, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (5) setp_ne p0, c4.wzx, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ne p0, c4.wzx, -v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (6) setp_ne p0, -v0.yzwy, c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ne p0, -v0.yzwy, c5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W0
	TESTEND

TESTCASE "setp - ne (7) setp_ne p0, -t0, -r5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ne p0, -t0, -r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (8) setp_ne p0, -t0.x, -r5";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ne p0, -t0.x, -r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (9) setp_ne p0, t0.y, r5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ne p0, t0.y, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - ne (10) setp_ne p0, r4.zxyw, r5.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ne p0, r4.zxyw, r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - ne (11) setp_ne p0, v0.xw, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ne p0, v0.xw, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (12) setp_ne p0, v0.wzx, c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ne p0, v0.wzx, c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W1
	TESTEND

TESTCASE "setp - ne (13) setp_ne p0, r4.x, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ne p0, r4.x, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (14) setp_ne p0, r4.yzwy, -r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ne p0, r4.yzwy, -r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (15) setp_ne p0, -r4.y, r5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ne p0, -r4.y, r5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (16) setp_ne p0, -t0.wzx, -r5.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ne p0, -t0.wzx, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (17) setp_ne p0, c4, v1.zxyw";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ne p0, c4, v1.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (18) setp_ne p0, -c4.zxyw, -v1";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ne p0, -c4.zxyw, -v1
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (19) setp_ne p0, -r4.xw, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ne p0, -r4.xw, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (20) setp_ne p0, -c4.xw, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ne p0, -c4.xw, -v1.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (21) setp_ne p0, c4.yzwy, -v1.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ne p0, c4.yzwy, -v1.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (22) setp_ne p0, -c4, -v1.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ne p0, -c4, -v1.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W1
	TESTEND

TESTCASE "setp - ne (23) setp_ne p0, -v0, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ne p0, -v0, -c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - ne (24) setp_ne p0, v0.x, -c5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ne p0, v0.x, -c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (25) setp_ne p0, c4.y, v1";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ne p0, c4.y, v1
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W1
	TESTEND

TESTCASE "setp - ne (26) setp_ne p0, v0.zxyw, -c5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ne p0, v0.zxyw, -c5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (27) setp_ne p0, -v0.xw, c5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ne p0, -v0.xw, c5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (28) setp_ne p0, -t0.wzx, r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ne p0, -t0.wzx, r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (29) setp_ne p0, t0.yzwy, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ne p0, t0.yzwy, -r5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (30) setp_ne p0, t0.x, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ne p0, t0.x, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (31) setp_ne p0, -t0.xw, r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ne p0, -t0.xw, r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (32) setp_ne p0, r4.wzx, r5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ne p0, r4.wzx, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (33) setp_ne p0, c4, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ne p0, c4, -v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (34) setp_ne p0, t0, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ne p0, t0, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (35) setp_ne p0, -r4.x, -r5.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ne p0, -r4.x, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (36) setp_ne p0, v0.x, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ne p0, v0.x, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (37) setp_ne p0, -v0.y, -c5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ne p0, -v0.y, -c5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (38) setp_ne p0, -t0.y, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ne p0, -t0.y, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (39) setp_ne p0, t0.y, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ne p0, t0.y, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W0
	TESTEND

TESTCASE "setp - ne (40) setp_ne p0, -v0.zxyw, c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ne p0, -v0.zxyw, c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (41) setp_ne p0, -c4.zxyw, v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ne p0, -c4.zxyw, v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (42) setp_ne p0, -c4.zxyw, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ne p0, -c4.zxyw, -v1.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (43) setp_ne p0, -c4.xw, -v1.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	setp_ne p0, -c4.xw, -v1.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (44) setp_ne p0, -r4.wzx, r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ne p0, -r4.wzx, r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - ne (45) setp_ne p0, -v0.wzx, -c5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	setp_ne p0, -v0.wzx, -c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (46) setp_ne p0, -r4.yzwy, -r5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ne p0, -r4.yzwy, -r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - ne (47) setp_ne p0, r4.yzwy, -r5.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	setp_ne p0, r4.yzwy, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z1 W0
	TESTEND

TESTCASE "setp - ne (48) setp_ne p0, -t0.yzwy, r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	setp_ne p0, -t0.yzwy, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// op * source registers * modifiers * swizzles
TESTCASE "setp_gt Mask (0) setp_gt p0.x, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_gt p0.x, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp_gt Mask (1) setp_gt p0.y, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_gt p0.y, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp_gt Mask (2) setp_gt p0.z, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_gt p0.z, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp_gt Mask (3) setp_gt p0.w, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_gt p0.w, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp_gt Mask (4) setp_gt p0.xz, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_gt p0.xz, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W0
	TESTEND

TESTCASE "setp_gt Mask (5) setp_gt p0.yw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_gt p0.yw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W1
	TESTEND

TESTCASE "setp_gt Mask (6) setp_gt p0.xyw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_gt p0.xyw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp_gt Mask (7) setp_gt p0.xyzw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_gt p0.xyzw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp_lt Mask (0) setp_lt p0.x, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_lt p0.x, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp_lt Mask (1) setp_lt p0.y, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_lt p0.y, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp_lt Mask (2) setp_lt p0.z, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_lt p0.z, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp_lt Mask (3) setp_lt p0.w, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_lt p0.w, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp_lt Mask (4) setp_lt p0.xz, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_lt p0.xz, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W0
	TESTEND

TESTCASE "setp_lt Mask (5) setp_lt p0.yw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_lt p0.yw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W1
	TESTEND

TESTCASE "setp_lt Mask (6) setp_lt p0.xyw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_lt p0.xyw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp_lt Mask (7) setp_lt p0.xyzw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_lt p0.xyzw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp_ge Mask (0) setp_ge p0.x, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ge p0.x, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp_ge Mask (1) setp_ge p0.y, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ge p0.y, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp_ge Mask (2) setp_ge p0.z, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ge p0.z, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp_ge Mask (3) setp_ge p0.w, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ge p0.w, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp_ge Mask (4) setp_ge p0.xz, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ge p0.xz, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W0
	TESTEND

TESTCASE "setp_ge Mask (5) setp_ge p0.yw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ge p0.yw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W1
	TESTEND

TESTCASE "setp_ge Mask (6) setp_ge p0.xyw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ge p0.xyw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp_ge Mask (7) setp_ge p0.xyzw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ge p0.xyzw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp_le Mask (0) setp_le p0.x, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_le p0.x, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp_le Mask (1) setp_le p0.y, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_le p0.y, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp_le Mask (2) setp_le p0.z, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_le p0.z, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp_le Mask (3) setp_le p0.w, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_le p0.w, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp_le Mask (4) setp_le p0.xz, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_le p0.xz, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W0
	TESTEND

TESTCASE "setp_le Mask (5) setp_le p0.yw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_le p0.yw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W1
	TESTEND

TESTCASE "setp_le Mask (6) setp_le p0.xyw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_le p0.xyw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp_le Mask (7) setp_le p0.xyzw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {1, 1, 1, 1};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_le p0.xyzw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp_eq Mask (0) setp_eq p0.x, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_eq p0.x, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp_eq Mask (1) setp_eq p0.y, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_eq p0.y, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp_eq Mask (2) setp_eq p0.z, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_eq p0.z, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp_eq Mask (3) setp_eq p0.w, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_eq p0.w, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp_eq Mask (4) setp_eq p0.xz, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_eq p0.xz, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W0
	TESTEND

TESTCASE "setp_eq Mask (5) setp_eq p0.yw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_eq p0.yw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W1
	TESTEND

TESTCASE "setp_eq Mask (6) setp_eq p0.xyw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_eq p0.xyw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp_eq Mask (7) setp_eq p0.xyzw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {0, 0, 0, 0};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_eq p0.xyzw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp_ne Mask (0) setp_ne p0.x, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ne p0.x, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp_ne Mask (1) setp_ne p0.y, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ne p0.y, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp_ne Mask (2) setp_ne p0.z, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ne p0.z, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp_ne Mask (3) setp_ne p0.w, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ne p0.w, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp_ne Mask (4) setp_ne p0.xz, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ne p0.xz, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W0
	TESTEND

TESTCASE "setp_ne Mask (5) setp_ne p0.yw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ne p0.yw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W1
	TESTEND

TESTCASE "setp_ne Mask (6) setp_ne p0.xyw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ne p0.xyw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp_ne Mask (7) setp_ne p0.xyzw, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1, 1, 1, 1};
	PixelShaderConstantF[5] = {0, 0, 0, 0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ne p0.xyzw, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// op * source registers * modifiers * swizzles
TESTCASE "setp - Mixed (0) setp_gt p0.x, r4, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_gt p0.x, r4, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (1) setp_lt p0.y, -c4.x, v1.x";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	CLEAR_P
	setp_lt p0.y, -c4.x, v1.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (2) setp_ge p0.z, v0.y, -c5.y";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_ge p0.z, v0.y, -c5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - Mixed (3) setp_le p0.w, -t0.zxyw, -r5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_le p0.w, -t0.zxyw, -r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (4) setp_eq p0.xz, -r4.xw, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_eq p0.xz, -r4.xw, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (5) setp_ne p0.yw, c4.wzx, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	CLEAR_P
	setp_ne p0.yw, c4.wzx, -v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W1
	TESTEND

TESTCASE "setp - Mixed (6) setp_gt p0.xyw, -v0.yzwy, c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_gt p0.xyw, -v0.yzwy, c5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (7) setp_ge p0.xyzw, t0, r5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_ge p0.xyzw, t0, r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - Mixed (8) setp_le p0.x, -c4.x, -v1.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	CLEAR_P
	setp_le p0.x, -c4.x, -v1.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (9) setp_eq p0.y, t0.y, -r5";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_eq p0.y, t0.y, -r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (10) setp_lt p0.z, r4.zxyw, r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_lt p0.z, r4.zxyw, r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - Mixed (11) setp_ne p0.w, v0.xw, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_ne p0.w, v0.xw, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp - Mixed (12) setp_gt p0.xz, -c4.wzx, v1.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	CLEAR_P
	setp_gt p0.xz, -c4.wzx, v1.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (13) setp_le p0.yw, -r4.yzwy, r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_le p0.yw, -r4.yzwy, r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W1
	TESTEND

TESTCASE "setp - Mixed (14) setp_lt p0.xyzw, -r4.yzwy, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_lt p0.xyzw, -r4.yzwy, -r5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W1
	TESTEND

TESTCASE "setp - Mixed (15) setp_eq p0.xyw, -c4, v1.y";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	CLEAR_P
	setp_eq p0.xyw, -c4, v1.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (16) setp_ge p0.xyw, r4.x, -r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ge p0.xyw, r4.x, -r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - Mixed (17) setp_ne p0.xz, t0.y, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_ne p0.xz, t0.y, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - Mixed (18) setp_le p0.z, t0.wzx, -r5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_le p0.z, t0.wzx, -r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - Mixed (19) setp_lt p0.x, v0.y, c5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_lt p0.x, v0.y, c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (20) setp_gt p0.y, -v0.zxyw, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_gt p0.y, -v0.zxyw, -c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (21) setp_lt p0.yw, -t0.xw, -r5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_lt p0.yw, -t0.xw, -r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (22) setp_ge p0.w, -c4.yzwy, v1.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	CLEAR_P
	setp_ge p0.w, -c4.yzwy, v1.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (23) setp_ne p0.xyzw, -c4.x, -v1.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	CLEAR_P
	setp_ne p0.xyzw, -c4.x, -v1.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z1 W1
	TESTEND

TESTCASE "setp - Mixed (24) setp_gt p0.z, -t0, -r5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_gt p0.z, -t0, -r5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (25) setp_eq p0.x, c4.zxyw, v1.x";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	CLEAR_P
	setp_eq p0.x, c4.zxyw, v1.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (26) setp_ne p0.y, r4.wzx, r5.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_ne p0.y, r4.wzx, r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (27) setp_gt p0.w, -r4.y, r5";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_gt p0.w, -r4.y, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (28) setp_le p0.xyw, v0.xw, -c5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_le p0.xyw, v0.xw, -c5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - Mixed (29) setp_le p0.xz, v0.yzwy, c5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_le p0.xz, v0.yzwy, c5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - Mixed (30) setp_eq p0.yw, v0, c5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_eq p0.yw, v0, c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (31) setp_eq p0.xyzw, v0.wzx, -c5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_eq p0.xyzw, v0.wzx, -c5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (32) setp_eq p0.z, r4.x, r5.zxyw";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_eq p0.z, r4.x, r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (33) setp_ne p0.xyw, -t0.zxyw, r5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_ne p0.xyw, -t0.zxyw, r5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - Mixed (34) setp_ge p0.x, -c4.xw, -v1.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	CLEAR_P
	setp_ge p0.x, -c4.xw, -v1.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (35) setp_ge p0.y, t0.yzwy, -r5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_ge p0.y, t0.yzwy, -r5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (36) setp_lt p0.w, -t0, -r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_lt p0.w, -t0, -r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (37) setp_ge p0.xz, v0.x, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_ge p0.xz, v0.x, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - Mixed (38) setp_gt p0.yw, -r4.y, -r5.x";
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_gt p0.yw, -r4.y, -r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (39) setp_gt p0.xyzw, t0.y, r5.y";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_gt p0.xyzw, t0.y, r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (40) setp_ge p0.x, t0.wzx, r5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_ge p0.x, t0.wzx, r5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (41) setp_ne p0.z, c4.yzwy, -v1";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	CLEAR_P
	setp_ne p0.z, c4.yzwy, -v1
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - Mixed (42) setp_le p0.xyw, c4.y, v1.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	CLEAR_P
	setp_le p0.xyw, c4.y, v1.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - Mixed (43) setp_ge p0.xz, v0.zxyw, -c5.y";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_ge p0.xz, v0.zxyw, -c5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W0
	TESTEND

TESTCASE "setp - Mixed (44) setp_le p0.y, -v0, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_le p0.y, -v0, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (45) setp_eq p0.w, -t0.x, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_eq p0.w, -t0.x, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (46) setp_le p0.xyzw, -v0.xw, c5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_le p0.xyzw, -v0.xw, c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W1
	TESTEND

TESTCASE "setp - Mixed (47) setp_lt p0.yw, t0.zxyw, r5.y";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_lt p0.yw, t0.zxyw, r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp - Mixed (48) setp_eq p0.x, -v0.yzwy, c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_eq p0.x, -v0.yzwy, c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (49) setp_ne p0.xz, t0, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_ne p0.xz, t0, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z1 W0
	TESTEND

TESTCASE "setp - Mixed (50) setp_lt p0.y, t0.zxyw, -r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_lt p0.y, t0.zxyw, -r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (51) setp_gt p0.w, v0.xw, -c5.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_gt p0.w, v0.xw, -c5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W1
	TESTEND

TESTCASE "setp - Mixed (52) setp_lt p0.xyw, r4.wzx, r5";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_lt p0.xyw, r4.wzx, r5
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (53) setp_ge p0.yw, v0.x, c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_ge p0.yw, v0.x, c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (54) setp_le p0.xyzw, -v0.zxyw, -c5";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_le p0.xyzw, -v0.zxyw, -c5
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (55) setp_lt p0.z, -r4.xw, r5.yzwy";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	mov r4, c4
	mov r5, c5
	CLEAR_P
	setp_lt p0.z, -r4.xw, r5.yzwy
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z1 W0
	TESTEND

TESTCASE "setp - Mixed (56) setp_ne p0.x, c4.y, -v1.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[4] = {1.25, 0.0, 2.0, -0.75};
	TESTBEGIN
	dcl v1
	CLEAR_P
	setp_ne p0.x, c4.y, -v1.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (57) setp_lt p0.xz, -t0.yzwy, -r5.y";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_lt p0.xz, -t0.yzwy, -r5.y
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (58) setp_ne p0.y, v0.xw, -c5.x";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_ne p0.y, v0.xw, -c5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (59) setp_ne p0.xyw, v0.wzx, -c5.zxyw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_ne p0.xyw, v0.wzx, -c5.zxyw
	mov r2, c0.y
	mov r2.gw, c0.x
	X1 Y1 Z0 W1
	TESTEND

TESTCASE "setp - Mixed (60) setp_gt p0.xz, -t0.x, -r5.x";
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl t0
	mov r5, c5
	CLEAR_P
	setp_gt p0.xz, -t0.x, -r5.x
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (61) setp_ge p0.w, v0.wzx, -c5.wzx";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_ge p0.w, v0.wzx, -c5.wzx
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y0 Z0 W0
	TESTEND

TESTCASE "setp - Mixed (62) setp_eq p0.xyw, -v0.yzwy, -c5.xw";
	USEARBSWZ
	PASSBEGIN
	PixelShaderConstantF[5] = {-1.25, 0.0, 2.0, 1.0};
	TESTBEGIN
	dcl v0
	CLEAR_P
	setp_eq p0.xyw, -v0.yzwy, -c5.xw
	mov r2, c0.y
	mov r2.gw, c0.x
	X0 Y1 Z0 W0
	TESTEND




////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

// End of file.  405 tests generated.