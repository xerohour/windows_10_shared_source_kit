// Tests for Instruction: crs
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
TESTCASE "Crs - Standard (0) crs r0.xyz, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.1171875, 0.455078125, 1.341796875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs r0.xyz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Standard (1) crs_pp r0.xyz, -c4_abs, v1_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.4453125, -0.455078125, 1.107421875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	crs_pp r0.xyz, -c4_abs, v1_abs
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Standard (2) crs r0.xyz, -v0_abs, -c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.4453125, 1.392578125, -1.341796875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	crs r0.xyz, -v0_abs, -c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Standard (3) crs_pp r0.xyz, -r4_abs, -r5_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.4453125, 0.455078125, -1.107421875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	crs_pp r0.xyz, -r4_abs, -r5_abs
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Standard (4) crs_pp r0.xyz, v0, c5_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.1171875, 1.392578125, 1.107421875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	crs_pp r0.xyz, v0, c5_abs
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Standard (5) crs r0.xyz, -c4, -v1_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.1171875, 1.392578125, 1.107421875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	crs r0.xyz, -c4, -v1_abs
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Standard (6) crs_pp r0.xyz, c4_abs, -v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.4453125, -1.392578125, 1.341796875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	crs_pp r0.xyz, c4_abs, -v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Sat (0) crs_sat r0.xyz, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.455078125, 1.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs_sat r0.xyz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Sat (1) crs_sat_pp r0.xyz, -c4_abs, v1_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	crs_sat_pp r0.xyz, -c4_abs, v1_abs
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Sat (2) crs_sat r0.xyz, -v0_abs, -c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	crs_sat r0.xyz, -v0_abs, -c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Sat (3) crs_sat_pp r0.xyz, -r4_abs, -r5_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.455078125, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	crs_sat_pp r0.xyz, -r4_abs, -r5_abs
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Sat (4) crs_sat_pp r0.xyz, v0, c5_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.0, 1.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	crs_sat_pp r0.xyz, v0, c5_abs
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Sat (5) crs_sat r0.xyz, -c4, -v1_abs"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 1.0, 1.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	crs_sat r0.xyz, -c4, -v1_abs
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Sat (6) crs_sat_pp r0.xyz, c4_abs, -v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 1.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	crs_sat_pp r0.xyz, c4_abs, -v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// masks & predication
TESTCASE "Crs - Pred/Mask (0) crs r0.x, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.1171875, 0.0, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs r0.x, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (1) (p0.x) crs r0.y, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.455078125, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) crs r0.y, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (2) (p0.y) crs r0.z, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.341796875, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) crs r0.z, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (3) (!p0.z) crs r0.xy, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) crs r0.xy, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (4) (!p0.w) crs r0.xz, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 2.1171875, 0.0, 1.341796875, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) crs r0.xz, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (5) crs r0.yz, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.455078125, 1.341796875, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	crs r0.yz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (6) (!p0) crs r0.xyz, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.455078125, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) crs r0.xyz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (7) (!p0.z) crs r0.x, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 2.1171875, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) crs r0.x, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (8) (!p0.y) crs r0.y, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) crs r0.y, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (9) (!p0.z) crs r0.z, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.341796875, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) crs r0.z, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (10) (p0) crs r0.xz, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 2.1171875, 0.0, 1.341796875, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) crs r0.xz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (11) (p0.w) crs r0.xy, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) crs r0.xy, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (12) crs r0.xyz, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.1171875, 0.455078125, 1.341796875, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	crs r0.xyz, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (13) (p0.y) crs r0.yz, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) crs r0.yz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (14) (p0) crs r0.y, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.455078125, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) crs r0.y, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (15) (!p0.x) crs r0.xy, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 2.1171875, 0.455078125, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) crs r0.xy, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (16) (p0.x) crs r0.x, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 2.1171875, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) crs r0.x, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (17) (p0) crs r0.yz, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) crs r0.yz, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (18) crs r0.z, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.341796875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs r0.z, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (19) (!p0.z) crs r0.xz, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) crs r0.xz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (20) (!p0.x) crs r0.xyz, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 2.1171875, 0.455078125, 1.341796875, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) crs r0.xyz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (21) (p0) crs r0.z, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.341796875, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) crs r0.z, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (22) crs r0.xz, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.1171875, 0.0, 1.341796875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs r0.xz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (23) (!p0.y) crs r0.x, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 2.1171875, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) crs r0.x, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (24) (!p0) crs r0.xy, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.455078125, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0) crs r0.xy, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (25) (!p0.w) crs r0.yz, c4, v1"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) crs r0.yz, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (26) (!p0.w) crs r0.xyz, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) crs r0.xyz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (27) (!p0.z) crs r0.y, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.455078125, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) crs r0.y, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (28) (p0.w) crs r0.x, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) crs r0.x, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (29) (!p0.y) crs r0.xyz, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 2.1171875, 0.455078125, 1.341796875, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) crs r0.xyz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (30) crs r0.y, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.455078125, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	crs r0.y, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (31) crs r0.xy, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.1171875, 0.455078125, 0.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	crs r0.xy, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (32) (!p0.x) crs r0.z, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) crs r0.z, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (33) (!p0.x) crs r0.xz, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 2.1171875, 0.0, 1.341796875, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) crs r0.xz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (34) (p0.z) crs r0.yz, v0, c5"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.455078125, 1.341796875, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) crs r0.yz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (35) (p0.x) crs r0.z, r4, r5"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) crs r0.z, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND



// End of file.  50 tests generated.