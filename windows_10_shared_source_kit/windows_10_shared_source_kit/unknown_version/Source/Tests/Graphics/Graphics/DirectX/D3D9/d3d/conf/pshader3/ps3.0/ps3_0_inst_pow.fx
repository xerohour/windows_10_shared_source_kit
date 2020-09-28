// Tests for Instruction: pow
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
TESTCASE "pow - Standard (0) pow r0, r4.x, r5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.593980421906, 0.593980421906, 0.593980421906, 0.593980421906
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	pow r0, r4.x, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (1) pow_pp r0, -c4_abs.y, -v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.15526394452, 1.15526394452, 1.15526394452, 1.15526394452
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	pow_pp r0, -c4_abs.y, -v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (2) pow r0, -v0_abs.z, c5_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.34375, 1.34375, 1.34375, 1.34375
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	pow r0, -v0_abs.z, c5_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (3) pow_pp r0, r4.w, -r5_abs.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.437980970344, 0.437980970344, 0.437980970344, 0.437980970344
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	pow_pp r0, r4.w, -r5_abs.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (4) pow_pp r0, -v0_abs.x, -c5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.20855015894, 1.20855015894, 1.20855015894, 1.20855015894
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	pow_pp r0, -v0_abs.x, -c5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (5) pow_pp r0, -v0_abs.y, -c5_abs.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.672398362026, 0.672398362026, 0.672398362026, 0.672398362026
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	pow_pp r0, -v0_abs.y, -c5_abs.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (6) pow r0, -c4_abs.z, v1.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.43346846782, 1.43346846782, 1.43346846782, 1.43346846782
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	pow r0, -c4_abs.z, v1.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (7) pow r0, -c4_abs.w, v1_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.96875, 1.96875, 1.96875, 1.96875
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	pow r0, -c4_abs.w, v1_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (8) pow r0, -r4_abs.y, r5.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.561403508772, 0.561403508772, 0.561403508772, 0.561403508772
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	pow r0, -r4_abs.y, r5.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (9) pow r0, r4.z, r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.928796094753, 0.928796094753, 0.928796094753, 0.928796094753
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	pow r0, r4.z, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (10) pow r0, c4.x, -v1_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.13333333333, 2.13333333333, 2.13333333333, 2.13333333333
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	pow r0, c4.x, -v1_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (11) pow_pp r0, v0_abs.y, c5.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.02102154946, 2.02102154946, 2.02102154946, 2.02102154946
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	pow_pp r0, v0_abs.y, c5.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (12) pow_pp r0, -c4.z, -v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.816170788436, 0.816170788436, 0.816170788436, 0.816170788436
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	pow_pp r0, -c4.z, -v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (13) pow r0, -v0.w, -c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.62768822599, 0.62768822599, 0.62768822599, 0.62768822599
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	pow r0, -v0.w, -c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (14) pow_pp r0, v0.x, -c5_abs.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.51791064613, 2.51791064613, 2.51791064613, 2.51791064613
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	pow_pp r0, v0.x, -c5_abs.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (15) pow_pp r0, -v0.w, -c5_abs.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.844213625275, 0.844213625275, 0.844213625275, 0.844213625275
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	pow_pp r0, -v0.w, -c5_abs.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Standard (16) pow_pp r0, r4.y, r5_abs.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.78125, 1.78125, 1.78125, 1.78125
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	pow_pp r0, r4.y, r5_abs.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// source registers * modifiers * swizzles
TESTCASE "pow - sat (0) pow_sat r0, r4.w, r5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.844213625275, 0.844213625275, 0.844213625275, 0.844213625275
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	pow_sat r0, r4.w, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - sat (1) pow_sat r0, c4.z, v1.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	pow_sat r0, c4.z, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - sat (2) pow_sat r0, v0.y, c5.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.561403508772, 0.561403508772, 0.561403508772, 0.561403508772
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	pow_sat r0, v0.y, c5.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - sat (3) pow_sat_pp r0, r4.y, r5.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.w
	pow_sat_pp r0, r4.y, r5.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - sat (4) pow_sat_pp r0, c4.w, v1.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.844213625275, 0.844213625275, 0.844213625275, 0.844213625275
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.w
	pow_sat_pp r0, c4.w, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - sat (5) pow_sat_pp r0, v0.y, c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.0, 1.0, 1.0, 1.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.w
	pow_sat_pp r0, v0.y, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// masks & predication
TESTCASE "pow - Pred/Mask (0) pow r0, r4.w, r5.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.507936507937, 0.507936507937, 0.507936507937, 0.507936507937
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	pow r0, r4.w, r5.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (1) (p0.x) pow r0.x, c4.w, v1.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.844213625275, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) pow r0.x, c4.w, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (2) (p0.y) pow r0.y, v0.w, c5.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.844213625275, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) pow r0.y, v0.w, c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (3) (!p0.z) pow r0.z, r4.x, r5.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) pow r0.z, r4.x, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (4) (!p0.w) pow r0.w, c4.y, v1.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.865603055254
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) pow r0.w, c4.y, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (5) pow r0.xz, v0.x, c5.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.397154681218, 0.0, 0.397154681218, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	pow r0.xz, v0.x, c5.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (6) (!p0) pow r0.yw, v0.x, c5.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 1.20855015894, 0.0, 1.20855015894
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) pow r0.yw, v0.x, c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (7) (p0.z) pow r0.xyw, c4.y, v1.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) pow r0.xyw, c4.y, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (8) (p0.y) pow r0.xyzw, c4.z, v1.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 1.2252337552, 1.2252337552, 1.2252337552, 1.2252337552
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) pow r0.xyzw, c4.z, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (9) (!p0.y) pow r0, v0.y, c5.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.561403508772, 0.561403508772, 0.561403508772, 0.561403508772
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) pow r0, v0.y, c5.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (10) (!p0.w) pow r0.x, r4.y, r5.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) pow r0.x, r4.y, r5.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (11) (!p0) pow r0.y, r4.w, r5.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.844213625275, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0) pow r0.y, r4.w, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (12) pow r0.z, c4.w, v1.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 2.28320422053, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	pow r0.z, c4.w, v1.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (13) (p0.y) pow r0.w, r4.z, r5.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.y) pow r0.w, r4.z, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (14) (p0) pow r0.xz, c4.y, v1.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.561403508772, 0.0, 0.561403508772, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) pow r0.xz, c4.y, v1.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (15) (p0.x) pow r0.yw, r4.z, r5.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 1.2252337552, 0.0, 1.2252337552
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) pow r0.yw, r4.z, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (16) (!p0.x) pow r0.xyw, r4.y, r5.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.865603055254, 0.865603055254, 0.0, 0.865603055254
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) pow r0.xyw, r4.y, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (17) pow r0.xyzw, v0.x, c5.y"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.20855015894, 1.20855015894, 1.20855015894, 1.20855015894
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	pow r0.xyzw, v0.x, c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (18) (p0.w) pow r0, c4.w, v1.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) pow r0, c4.w, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (19) (!p0) pow r0.z, v0.y, c5.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 1.48721361692, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) pow r0.z, v0.y, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (20) (p0) pow r0.x, v0.z, c5.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.744186046512, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0) pow r0.x, v0.z, c5.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (21) (p0.z) pow r0.xz, r4.z, r5.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) pow r0.xz, r4.z, r5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (22) pow r0.y, c4.z, v1.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.744186046512, 0.0, 0.0
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	pow r0.y, c4.z, v1.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (23) pow r0.w, v0.w, c5.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.507936507937
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	pow r0.w, v0.w, c5.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (24) pow r0.yw, c4.x, v1.z"
	PASSBEGIN
	TESTBEGIN
	def c1, 0.0, 2.13333333333, 0.0, 2.13333333333
	dcl_texcoord1 v1
	mov r0, c0.y
	mov r2, c0.z
	pow r0.yw, c4.x, v1.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (25) (!p0.w) pow r0.xyw, v0.y, c5.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 1.48721361692, 1.48721361692, 0.0, 1.48721361692
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) pow r0.xyw, v0.y, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (26) (!p0) pow r0.xyzw, r4.w, r5.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.507936507937, 0.0, 0.507936507937
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0) pow r0.xyzw, r4.w, r5.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (27) (p0.z) pow r0.y, r4.y, r5.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.561403508772, 0.0, 0.0
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(p0.z) pow r0.y, r4.y, r5.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (28) (!p0.x) pow r0, c4.y, v1.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 2.02102154946, 2.02102154946, 2.02102154946, 2.02102154946
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) pow r0, c4.y, v1.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (29) (!p0.x) pow r0.w, v0.w, c5.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) pow r0.w, v0.w, c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (30) (!p0) pow r0.xyw, v0.z, c5.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0) pow r0.xyw, v0.z, c5.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (31) pow r0.x, v0.y, c5.w"
	PASSBEGIN
	TESTBEGIN
	def c1, 2.02102154946, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	pow r0.x, v0.y, c5.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (32) (!p0.x) pow r0.z, c4.z, v1.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.x) pow r0.z, c4.z, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (33) (!p0.y) pow r0.xz, v0.x, c5.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 0, 1, 0};
	TESTBEGIN
	def c1, 0.593980421906, 0.0, 0.593980421906, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) pow r0.xz, v0.x, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (34) (!p0.z) pow r0.yw, r4.y, r5.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.865603055254, 0.0, 0.865603055254
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) pow r0.yw, r4.y, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (35) (p0.x) pow r0.xyzw, c4.z, v1.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) pow r0.xyzw, c4.z, v1.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (36) (!p0.z) pow r0, c4.z, v1.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) pow r0, c4.z, v1.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (37) (p0.x) pow r0.y, c4.z, v1.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.0, 1.43346846782, 0.0, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.x) pow r0.y, c4.z, v1.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (38) pow r0.xyw, v0.y, c5.x"
	PASSBEGIN
	TESTBEGIN
	def c1, 1.48721361692, 1.48721361692, 0.0, 1.48721361692
	dcl_texcoord0 v0
	mov r0, c0.y
	mov r2, c0.z
	pow r0.xyw, v0.y, c5.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (39) (!p0.z) pow r0.w, c4.x, v1.x"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.593980421906
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) pow r0.w, c4.x, v1.x
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (40) (p0.w) pow r0.xz, c4.z, v1.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 1, 0, 1};
	TESTBEGIN
	def c1, 0.928796094753, 0.0, 0.928796094753, 0.0
	dcl_texcoord1 v1
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) pow r0.xz, c4.z, v1.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (41) (!p0.z) pow r0.x, v0.w, c5.w"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 2.28320422053, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.z) pow r0.x, v0.w, c5.w
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (42) (!p0.y) pow r0.z, v0.w, c5.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.844213625275, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(!p0.y) pow r0.z, v0.w, c5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (43) (p0.w) pow r0.yw, v0.z, c5.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.0, 0.0, 0.0, 0.0
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) pow r0.yw, v0.z, c5.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (44) (!p0.w) pow r0.xyzw, r4.w, r5.y"
	PASSBEGIN
	PixelShaderConstantF[2] = {0, 0, 0, 0};
	TESTBEGIN
	def c1, 0.844213625275, 0.844213625275, 0.844213625275, 0.844213625275
	SET_PRED
	mov r4, c4
	mov r5, c5
	mov r0, c0.y
	mov r2, c0.z
	(!p0.w) pow r0.xyzw, r4.w, r5.y
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "pow - Pred/Mask (45) (p0.w) pow r0, v0.z, c5.z"
	PASSBEGIN
	PixelShaderConstantF[2] = {1, 1, 1, 1};
	TESTBEGIN
	def c1, 0.744186046512, 0.744186046512, 0.744186046512, 0.744186046512
	dcl_texcoord0 v0
	SET_PRED
	mov r0, c0.y
	mov r2, c0.z
	(p0.w) pow r0, v0.z, c5.z
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND



// End of file.  69 tests generated.