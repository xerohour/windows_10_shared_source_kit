_Z V_W
	TESTEND

TESTCASE "mNxN - Values (8) m3x3 r0.xyz, r4.x, c5"
	PASSBEGIN
	PixelShaderConstantF[4] = {-2.28125, 2.5625, -1.28125, -3.90625};
	PixelShaderConstantF[5] = {0.9375, -2.53125, -1.65625, 0};
	PixelShaderConstantF[6] = {-1.71875, 2.125, 2.59375, 0};
	PixelShaderConstantF[7] = {-0.75, -2.625, -2.1875, 0};
	TESTBEGIN
	def c1, 7.4140625, -6.84375, 12.689453125, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	m3x3 r0.xyz, r4.x, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mNxN - Values (9) m3x2 r0.xy, r4.y, c5"
	PASSBEGIN
	PixelShaderConstantF[4] = {3.46875, 0.65625, 1.9375, -2.875};
	PixelShaderConstantF[5] = {0.375, 3.375, -2.78125, 0};
	PixelShaderConstantF[6] = {0.96875, 1.78125, 1.53125, 0};
	TESTBEGIN
	def c1, 0.6357421875, 2.8095703125, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	m3x2 r0.xy, r4.y, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mNxN - Values (10) m3x2 r0.xy, -r4.zxyw, c5"
	PASSBEGIN
	PixelShaderConstantF[4] = {-2.28125, 2.5625, -1.28125, -3.90625};
	PixelShaderConstantF[5] = {0.9375, -2.53125, -1.65625, 0};
	PixelShaderConstantF[6] = {-1.71875, 2.125, 2.59375, 0};
	TESTBEGIN
	def c1, -0.3291015625, -4.0009765625, 0.0, 0.0
	mov r4, c4
	mov r0, c0.y
	mov r2, c0.z
	m3x2 r0.xy, -r4.zxyw, c5
	VERIFY_RESULT
	V_X V_Y V_Z V_W
	TESTEND

TESTCASE "mNxN - Values (11) m4x3 r0.xyz, -r4.x, c5"
	PASSBEGIN
	PixelShaderConstantF[4] = {3.46875, 0.65625, 1.9375, -2.875};
	PixelShaderConstantF[5] = {0.375, 3.375, -2.78125, 3.75};
	PixelShaderConstantF[6] = {0.96875, 1.781