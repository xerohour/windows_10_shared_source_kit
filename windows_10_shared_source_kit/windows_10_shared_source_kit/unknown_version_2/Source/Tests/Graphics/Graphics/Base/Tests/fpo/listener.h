s care of the code generation:
	inst = Inst_Setp(op, dest, sources, result, verifymask)
	inst.WriteTest()

	curCount += 1

	return inst.GetText()

$}$#################################################################################################

#define PS_SETUP \
	$ps_ver_token \
	def c0, 1.0, 0.0, -1.0, 0.0

// following line sets p0 to 0000, mainly for write mask tests
// we can rely on this since setp_eq will be tested separately
#define CLEAR_P		setp_eq p0, c0.x, c0.y

// Test macros.  These 