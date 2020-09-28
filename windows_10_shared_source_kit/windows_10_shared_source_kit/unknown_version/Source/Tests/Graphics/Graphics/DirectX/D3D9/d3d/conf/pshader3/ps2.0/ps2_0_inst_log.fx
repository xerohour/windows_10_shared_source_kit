// Tests for Instruction: log
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
	PixelShaderConstantF[4] = {-0.46875, -1.78125, 1.34375, 1.96875};

VertexShader NormalVS =
	asm
	{
		vs_2_0
		dcl_position	v0
		def c5, -0.46875, -1.78125, 1.34375, 1.96875
		def c6, 0.34375, 0.125, 0.5, 0.609375
		m4x4 oPos, v0, c0
		mov oT0, c5
		mov oD0, c6
	};


// Tests:

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * modifiers * swizzles
TESTCASE "Log - Standard (0) log r0, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.09310940439, -1.09310940439, -1.09310940439, -1.09310940439
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	log r0, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Standard (1) log_pp r0, c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.832890014165, 0.832890014165, 0.832890014165, 0.832890014165
	mov r0, c0.y
	mov r2, c0.w
	log_pp r0, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Standard (2) log r0, -v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.0, -1.0, -1.0, -1.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log r0, -v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Standard (3) log_pp r0, -t0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.9772799235, 0.9772799235, 0.9772799235, 0.9772799235
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	log_pp r0, -t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Standard (4) log_pp r0, -c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.09310940439, -1.09310940439, -1.09310940439, -1.09310940439
	mov r0, c0.y
	mov r2, c0.w
	log_pp r0, -c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Standard (5) log_pp r0, -r4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.832890014165, 0.832890014165, 0.832890014165, 0.832890014165
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	log_pp r0, -r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Standard (6) log r0, t0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.426264754702, 0.426264754702, 0.426264754702, 0.426264754702
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	log r0, t0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Standard (7) log r0, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.714597781138, -0.714597781138, -0.714597781138, -0.714597781138
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log r0, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Standard (8) log r0, v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.0, -3.0, -3.0, -3.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log r0, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Standard (9) log r0, -c4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.426264754702, 0.426264754702, 0.426264754702, 0.426264754702
	mov r0, c0.y
	mov r2, c0.z
	log r0, -c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Standard (10) log_pp r0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.54056838136, -1.54056838136, -1.54056838136, -1.54056838136
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log_pp r0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Standard (11) log_pp r0, r4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.426264754702, 0.426264754702, 0.426264754702, 0.426264754702
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	log_pp r0, r4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Standard (12) log_pp r0, -t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.09310940439, -1.09310940439, -1.09310940439, -1.09310940439
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	log_pp r0, -t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Standard (13) log_pp r0, t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.832890014165, 0.832890014165, 0.832890014165, 0.832890014165
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	log_pp r0, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Standard (14) log_pp r0, -r4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.9772799235, 0.9772799235, 0.9772799235, 0.9772799235
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	log_pp r0, -r4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Standard (15) log r0, -c4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.9772799235, 0.9772799235, 0.9772799235, 0.9772799235
	mov r0, c0.y
	mov r2, c0.z
	log r0, -c4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * modifiers * swizzles
TESTCASE "Log - sat (0) log_sat r0, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	log_sat r0, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - sat (1) log_sat r0, c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.832890014165, 0.832890014165, 0.832890014165, 0.832890014165
	mov r0, c0.y
	mov r2, c0.z
	log_sat r0, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - sat (2) log_sat r0, v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log_sat r0, v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - sat (3) log_sat r0, t0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.9772799235, 0.9772799235, 0.9772799235, 0.9772799235
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	log_sat r0, t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - sat (4) log_sat_pp r0, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	mov r0, c0.y
	mov r2, c0.w
	log_sat_pp r0, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - sat (5) log_sat_pp r0, r4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.832890014165, 0.832890014165, 0.832890014165, 0.832890014165
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.w
	log_sat_pp r0, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - sat (6) log_sat_pp r0, t0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.426264754702, 0.426264754702, 0.426264754702, 0.426264754702
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	log_sat_pp r0, t0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - sat (7) log_sat_pp r0, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log_sat_pp r0, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - sat (8) log_sat_pp r0, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log_sat_pp r0, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - sat (9) log_sat_pp r0, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	log_sat_pp r0, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - sat (10) log_sat_pp r0, v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log_sat_pp r0, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - sat (11) log_sat_pp r0, t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.832890014165, 0.832890014165, 0.832890014165, 0.832890014165
	dcl t0
	mov r0, c0.y
	mov r2, c0.w
	log_sat_pp r0, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - sat (12) log_sat r0, r4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.426264754702, 0.426264754702, 0.426264754702, 0.426264754702
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	log_sat r0, r4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - sat (13) log_sat_pp r0, c4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.426264754702, 0.426264754702, 0.426264754702, 0.426264754702
	mov r0, c0.y
	mov r2, c0.w
	log_sat_pp r0, c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - sat (14) log_sat r0, r4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.9772799235, 0.9772799235, 0.9772799235, 0.9772799235
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	log_sat r0, r4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - sat (15) log_sat r0, c4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.9772799235, 0.9772799235, 0.9772799235, 0.9772799235
	mov r0, c0.y
	mov r2, c0.z
	log_sat r0, c4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// masks & predication
TESTCASE "Log - Pred/Mask (0) log r0, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.09310940439, -1.09310940439, -1.09310940439, -1.09310940439
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	log r0, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (1) log r0.x, r4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.832890014165, 0.0, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	log r0.x, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (2) log r0.y, r4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.426264754702, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	log r0.y, r4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (3) log r0.z, r4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.9772799235, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	log r0.z, r4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (4) log r0.w, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -1.09310940439
	mov r0, c0.y
	mov r2, c0.z
	log r0.w, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (5) log r0.xz, c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.832890014165, 0.0, 0.832890014165, 0.0
	mov r0, c0.y
	mov r2, c0.z
	log r0.xz, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (6) log r0.yw, c4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.426264754702, 0.0, 0.426264754702
	mov r0, c0.y
	mov r2, c0.z
	log r0.yw, c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (7) log r0.xyw, c4.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.9772799235, 0.9772799235, 0.0, 0.9772799235
	mov r0, c0.y
	mov r2, c0.z
	log r0.xyw, c4.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (8) log r0.xyzw, v0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.54056838136, -1.54056838136, -1.54056838136, -1.54056838136
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log r0.xyzw, v0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (9) log r0, v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.0, -3.0, -3.0, -3.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log r0, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (10) log r0.x, v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.0, 0.0, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log r0.x, v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (11) log r0.y, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -0.714597781138, 0.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log r0.y, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (12) log r0.z, t0.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -1.09310940439, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	log r0.z, t0.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (13) log r0.w, t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.832890014165
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	log r0.w, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (14) log r0.xz, t0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.426264754702, 0.0, 0.426264754702, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	log r0.xz, t0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (15) log r0.yw, t0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.9772799235, 0.0, 0.9772799235
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	log r0.yw, t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (16) log r0.xyw, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.09310940439, -1.09310940439, 0.0, -1.09310940439
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	log r0.xyw, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (17) log r0.xyzw, r4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.832890014165, 0.832890014165, 0.832890014165, 0.832890014165
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	log r0.xyzw, r4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (18) log r0.x, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.09310940439, 0.0, 0.0, 0.0
	mov r0, c0.y
	mov r2, c0.z
	log r0.x, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (19) log r0.y, c4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.09310940439, 0.0, 0.0
	mov r0, c0.y
	mov r2, c0.z
	log r0.y, c4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (20) log r0.xz, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, -1.09310940439, 0.0, -1.09310940439, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	log r0.xz, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (21) log r0.yw, r4.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -1.09310940439, 0.0, -1.09310940439
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	log r0.yw, r4.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (22) log r0.z, c4.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.832890014165, 0.0
	mov r0, c0.y
	mov r2, c0.z
	log r0.z, c4.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (23) log r0.xyw, v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, -3.0, -3.0, 0.0, -3.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log r0.xyw, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (24) log r0, c4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.426264754702, 0.426264754702, 0.426264754702, 0.426264754702
	mov r0, c0.y
	mov r2, c0.z
	log r0, c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (25) log r0.w, r4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.426264754702
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	log r0.w, r4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (26) log r0.xyzw, c4.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.426264754702, 0.426264754702, 0.426264754702, 0.426264754702
	mov r0, c0.y
	mov r2, c0.z
	log r0.xyzw, c4.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (27) log r0.y, t0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.832890014165, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	log r0.y, t0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (28) log r0.yw, v0.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, -3.0, 0.0, -3.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log r0.yw, v0.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (29) log r0.z, v0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, -1.0, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log r0.z, v0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (30) log r0.xyw, t0.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.426264754702, 0.426264754702, 0.0, 0.426264754702
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	log r0.xyw, t0.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (31) log r0, t0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.9772799235, 0.9772799235, 0.9772799235, 0.9772799235
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	log r0, t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (32) log r0.x, t0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.9772799235, 0.0, 0.0, 0.0
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	log r0.x, t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (33) log r0.w, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, -0.714597781138
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log r0.w, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (34) log r0.xz, v0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, -0.714597781138, 0.0, -0.714597781138, 0.0
	dcl v0
	mov r0, c0.y
	mov r2, c0.w
	log r0.xz, v0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "Log - Pred/Mask (35) log r0.xyzw, t0.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.9772799235, 0.9772799235, 0.9772799235, 0.9772799235
	dcl t0
	mov r0, c0.y
	mov r2, c0.z
	log r0.xyzw, t0.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  68 tests generated.