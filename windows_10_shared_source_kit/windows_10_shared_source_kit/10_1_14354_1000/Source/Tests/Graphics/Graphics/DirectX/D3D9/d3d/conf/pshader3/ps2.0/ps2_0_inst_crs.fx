// Tests for Instruction: crs
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

TESTCASE "Crs - Standard (1) crs_pp r0.xyz, c4, -v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.05859375, -0.6962890625, -0.5537109375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	crs_pp r0.xyz, c4, -v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Standard (2) crs r0.xyz, -v0, -c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.72265625, 0.6962890625, -0.6708984375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	crs r0.xyz, -v0, -c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Standard (3) crs_pp r0.xyz, -t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.1171875, -0.455078125, -1.341796875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	crs_pp r0.xyz, -t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Standard (4) crs r0.xyz, -c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.05859375, -0.6962890625, -0.5537109375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	crs r0.xyz, -c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Standard (5) crs r0.xyz, t0, -r5"
	PASSBEGIN
	TESTBEGIN
	def c1, -2.1171875, -0.455078125, -1.341796875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs r0.xyz, t0, -r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Standard (6) crs_pp r0.xyz, -r4, -r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.1171875, 0.455078125, 1.341796875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	crs_pp r0.xyz, -r4, -r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Standard (7) crs_pp r0.xyz, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.72265625, 0.6962890625, -0.6708984375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	crs_pp r0.xyz, v0, c5
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

TESTCASE "Crs - Sat (1) crs_sat_pp r0.xyz, c4, -v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	crs_sat_pp r0.xyz, c4, -v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Sat (2) crs_sat r0.xyz, -v0, -c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.6962890625, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	crs_sat r0.xyz, -v0, -c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Sat (3) crs_sat_pp r0.xyz, -t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	crs_sat_pp r0.xyz, -t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Sat (4) crs_sat r0.xyz, -c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.0, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	crs_sat r0.xyz, -c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Sat (5) crs_sat r0.xyz, t0, -r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs_sat r0.xyz, t0, -r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Sat (6) crs_sat_pp r0.xyz, -r4, -r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 0.455078125, 1.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	crs_sat_pp r0.xyz, -r4, -r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Sat (7) crs_sat_pp r0.xyz, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.6962890625, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	crs_sat_pp r0.xyz, v0, c5
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

TESTCASE "Crs - Pred/Mask (1) crs r0.x, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.05859375, 0.0, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	crs r0.x, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (2) crs r0.x, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.72265625, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	crs r0.x, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (3) crs r0.x, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.1171875, 0.0, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs r0.x, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (4) crs r0.y, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.455078125, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs r0.y, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (5) crs r0.y, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.6962890625, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	crs r0.y, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (6) crs r0.y, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.6962890625, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	crs r0.y, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (7) crs r0.y, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.455078125, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs r0.y, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (8) crs r0.z, r4, r5"
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

TESTCASE "Crs - Pred/Mask (9) crs r0.z, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.5537109375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	crs r0.z, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (10) crs r0.z, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -0.6708984375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	crs r0.z, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (11) crs r0.z, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 1.341796875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs r0.z, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (12) crs r0.xy, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.1171875, 0.455078125, 0.0, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs r0.xy, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (13) crs r0.xy, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.05859375, 0.6962890625, 0.0, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	crs r0.xy, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (14) crs r0.xy, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.72265625, 0.6962890625, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	crs r0.xy, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (15) crs r0.xy, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.1171875, 0.455078125, 0.0, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs r0.xy, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (16) crs r0.xz, r4, r5"
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

TESTCASE "Crs - Pred/Mask (17) crs r0.xz, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.05859375, 0.0, 0.5537109375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	crs r0.xz, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (18) crs r0.xz, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.72265625, 0.0, -0.6708984375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	crs r0.xz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (19) crs r0.xz, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.1171875, 0.0, 1.341796875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs r0.xz, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (20) crs r0.yz, r4, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.455078125, 1.341796875, 0.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs r0.yz, r4, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (21) crs r0.yz, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.6962890625, 0.5537109375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	crs r0.yz, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (22) crs r0.yz, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.6962890625, -0.6708984375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	crs r0.yz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (23) crs r0.yz, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.455078125, 1.341796875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs r0.yz, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (24) crs r0.xyz, r4, r5"
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

TESTCASE "Crs - Pred/Mask (25) crs r0.xyz, c4, v1"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.05859375, 0.6962890625, 0.5537109375, 0.0
	dcl v1
	mov r0, c0.y
	mov r2, c0.w
	crs r0.xyz, c4, v1
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (26) crs r0.xyz, v0, c5"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.72265625, 0.6962890625, -0.6708984375, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	crs r0.xyz, v0, c5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND

TESTCASE "Crs - Pred/Mask (27) crs r0.xyz, t0, r5"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.1171875, 0.455078125, 1.341796875, 0.0
	dcl t0
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	crs r0.xyz, t0, r5
	VERIFY_RESULT
	V_X V_Y V_Z
	TESTEND



// End of file.  44 tests generated.