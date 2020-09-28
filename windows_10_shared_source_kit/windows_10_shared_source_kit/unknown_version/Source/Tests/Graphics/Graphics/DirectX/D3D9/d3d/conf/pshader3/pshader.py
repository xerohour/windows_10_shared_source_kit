"""
PShader.py - Functions, Classes, and Data to aid in generating PShader test code.

Classes:
	Instruction - standard test generation class
	Operation - describe operation and compute result
	Predication - helps with instruction predication
	Parameter - base for source and destination parameters
		Destination - keeps track of normal destination with mask
		Source - standard 4 component source with modifiers/swizzles
			ScalarSource - source used as scalar
			MatrixSource - source used as first index of multiple registers for matrix
Functions:
	SetVersion - Setup version info variables - Must do this before using this module.
	SetupDefaults - Store default register values for later reference
	TranslateSource, CollapseSources, DefaultSrc - Help prepare source args
	DotProduct, Factorial - math utils
	FillMask, IntersectMasks - manipulate masks
	SimplifySwizzle, FillSwiz, Swiz - manipulate swizzles
	RandSwizzle - generate random swizzle that isn't in lstUsedSwizzles
	DescribeSource - describe a source with modifiers
	ApplySrcMod - Apply source modifiers
	ApplyPredMod - Apply predicate modifiers
	Saturate - return a saturated result vector (clamped between 0 and 1)
	ComputeCombinations - compute pairwise combinations of factors
	WriteVS - write specialized vertex shader
	SetGroup, GroupIndex - keep track of groups and number of tests written
	SetupSwizzles - called from SetVersion to setup swizzle lists
Data includes the following stuff:
	ps / vs version info and settings based on these
	maps for translating character based values
	lists of values for masks, swizzles, and predicates
	epselons for result testing
	group name and test counter
"""

import random, string
import math
from math import exp, ldexp, log, pow, sin, cos, tan, sqrt, pi
import operator
from StringIO import StringIO


# Some things are based on random numbers.  Since we want the same generated results
# each time, set the seed to a constant number here:
random.seed(1000)

# default value for a result register component:
default_result = 0.0

# Dictionary that holds default values for various registers
# SetupDefaults() fills this in
defaults = {}

# Start r# sources at r4 by default, and c# for sources at c4:
r_SourceStartIndex = 4
c_SourceStartIndex = 4

def SetVersion(version):
	"Sets up version variables, takes version string, ex: '2.x'"
	global ps_ver_token, vs_ver_token
	global ps_2_0, ps_2_x, ps_3_0
	global ps_ver, vs_ver
	global ps_version, vs_version
	global use_arbitrary_swizzles
	global predicate_support
	ps_version = version
	ps_ver_token = 'ps_' + ps_version.replace('.', '_')
	ps_3_0 = ps_version == '3.0'
	ps_2_x = ps_version == '2.x'
	ps_2_0 = ps_version == '2.0'
	ps_ver = float(ps_version.replace('x', '5'))
	if ps_ver < 3.0:
		vs_ver = 2.0
		vs_version = '2.0'
	else:
		vs_ver = 3.0
		vs_version = '3.0'
	vs_ver_token = 'vs_' + vs_version.replace('.', '_')
	# don't write testcases with arbitrary swizzles for 2.0:
	use_arbitrary_swizzles = ps_ver > 2.0
	predicate_support = ps_ver > 2.0
	SetupSwizzles()

def SetupDefaults(normdefs, coldefs):
	global defaults
	defaults = {'c0': [1.0, 0.0, eps, eps_pp]}
	for n,d in zip(range(len(normdefs)), normdefs):
		if ps_ver < 3.0:
			v_reg = 't'
		else:
			v_reg = 'v'
		defaults.update({
			'r%d' % (n + r_SourceStartIndex): d,
			'c%d' % (n + c_SourceStartIndex): d,
			'%s%d' % (v_reg, n): d })
	if ps_ver < 3.0:
		for n,c in zip(range(len(coldefs)), coldefs):
			defaults['v%d' % n] = c
def TranslateSource(reg_type, index):
	if len(reg_type) > 1:
		return reg_type
	if reg_type == 'r':
		index += r_SourceStartIndex
	elif reg_type in 'cbi':
		index += c_SourceStartIndex
	elif ps_ver < 3.0 and reg_type == 'v':
		index = index % 2
	return reg_type + str(index)
def CollapseSources(dct, count):
	"Collapse source variables to list and translate registers"
	sources = []
	for n in range(count):
		sources.append([
			dct['reg%d'%n],		# reg# - register
			dct['src%d'%n],		# src# - value
			dct['neg%d'%n],		# neg# - negate
			dct['abs%d'%n],		# abs# - _abs source mod
			dct['swz%d'%n]])	# swz# - swizzle
	LimitReadPorts(sources)
	for n in range(count):
		sources[n][0] = TranslateSource(sources[n][0], n)
	return sources
def ValidateSources(sources):
	"""Try to enforce some validation rules on list of sources
	X6045: When constant registers are read multiple times in a single instruction,
		the _abs modifier must either be present on all of the constants, or none of them.
	"""
	creg = None
	for src in sources:
		if src[0][0] in 'cbi':
			if creg:
				src[3] = creg[3]	# make subsequent _abs modifiers on constant register same as first
			else:
				creg = src

def LimitReadPorts(sources):
	"Fills in reg indexes to avoid read port limits if possible"
	# Keep track of register read from each read port:
	vtreg = None
	creg = None
	for src,n in zip(sources, range(len(sources))):
		r = src[0]
		if r[0] in 'vt':
			if not vtreg:
				vtreg = TranslateSource(r, n)
				src[0] = vtreg
			else:
				if len(r) == 1:				# actual register not specified, only type
					if vtreg[0] == r[0]:	# same type as stored earlier
						src[0] = vtreg		# use same register, so we don't hit readport limit
		elif r[0] in 'cbi':
			if not creg:
				creg = TranslateSource(r, n)
				src[0] = creg
			else:
				if len(r) == 1:				# actual register not specified, only type
					if creg[0] == r[0]:		# same type as stored earlier
						src[0] = creg		# use same register, so we don't hit readport limit

def DotProduct(a,b):
	return reduce(operator.add, map(operator.mul, a, b))

def Factorial(n):
	return reduce(operator.add, range(1,n+1))

def FillMask(mask):
	"Converts a 'xz' format mask into [1,0,1,0] format"
	if mask:
		if type(mask) is list:
			return mask[:]
		else:
			used = [SwizzleMap[c] for c in mask]
			return [n in used for n in range(4)]
	return [1]*4
def IntersectMasks(first, second):
	return [a and b for a,b in zip(FillMask(first), FillMask(second))]

def SimplifySwizzle(swz):
	"""SimplifySwizzle('xyww') --> 'xyw'
	remove all but the last replicated component for shortest equivalent swizzle"""
	if len(swz) < 2:
		return swz
	last = SwizzleMap[swz[-1]]
	for n in range(len(swz)-2, -1, -1):
		if SwizzleMap[swz[n]] != last:
			break
	return swz[:n+2]
def FillSwiz(swz):
	"Fill out swizzles into len 4 array of components"
	if not swz:
		swz = range(4)
	elif type(swz) == str:
		swz = [SwizzleMap[s] for s in swz]
	elif type(swz) is list:
		swz = swz[:]
	else:
		assert 0, 'wrong swz type'
	swz += [swz[-1] for n in range(4-len(swz))]
	return swz
def Swiz(val, swz):
	"""Perform the swizzle on the values and return the result:
	val is a len 4 array of values, swz is a len 0 to 4 string of swizzle characters:"""
	return [val[s] for s in FillSwiz(swz)]

def IsSwizzleArbitrary(swz):
	return not FillSwiz(swz) in map(FillSwiz, lstSimpleSwizzles)

# Tries to randomly generate a swizzle that hasn't been used yet:
def RandSwizzle(tries):
	for n in range(tries):		# try <tries> times to get a unique swizzle
		s = SimplifySwizzle(''.join([random.choice('xyzw') for n in range(random.randrange(4)+1)]))
		if s not in lstUsedSwizzles:
			lstUsedSwizzles.append(s)
			return s

def DescribeSource(_abs, neg, swz):
	"Return a description of the source with %s in place of the register"
	desc = '%s'
	if _abs:	desc += '_abs'
	if neg:		desc = '-' + desc
	if swz:		desc += '.' + swz
	return desc
def ApplySrcMod(src, _abs, neg, swz):
	"ApplySrcMod(src, abs, neg, swz) -> value"
	src = src[:]
	if _abs:	src = map(abs, src)
	if neg:		src = [-c for c in src]
	if swz:		src = Swiz(src, swz)
	return src

def ApplyPredMod(pred, pnot, pswz):
	if pred:
		pred = pred[:]
		if pnot:	pred = [not v for v in pred]
		if pswz:	pred = Swiz(pred, pswz)
	else:
		pred = [1]*4
	return pred

def Saturate(res, sat):
	res = res[:]
	if sat:
		for n in range(len(res)):
			if res[n] < 0.0: res[n] = 0.0
			elif res[n] > 1.0: res[n] = 1.0
	return res

def ComputeCombinations(params, max = 1000):
	import Pairs
	combos = Pairs.ComputeCombinations(params, max)
	# for each None result, fill in with a random value:
	for c in combos:
		for k,v in c.items():
			if v is None:
				c[k] = random.choice(params[k][0])
	return combos



class Instruction:
	def __init__(s, op, pred, dest, sources, result):
		s.op, s.pred, s.dest, s.sources = op, pred, dest, sources
		s.result = result
		s.outputmask = IntersectMasks(s.pred.Transform(), s.dest.mask)
		s.maskedresult = []
		s.decld = {}
		s.DeclareConstantsPerTestcase = False
		s.pp_eps = s.op.pp
		if not s.pp_eps and ps_ver < 3.0:
			for src in sources:
				if src.reg[0] == 'v':		# use partial precision epselon with color registers on 2.x
					s.pp_eps = 1
					break
		for n in range(len(s.result)):
			if s.outputmask[n]:
				s.maskedresult.append(s.result[n])
			else:
				s.maskedresult.append(default_result)
		for src in s.sources:
			if IsSwizzleArbitrary(src.swz):
				s.arbSwizzle = 1
				break
		else:
			s.arbSwizzle = 0
		s.out = StringIO()
	def __str__(s):
		return '%s%s %s, %s' % (s.pred, s.op, s.dest, ', '.join([str(i) for i in s.sources]))
	def write(s, line):
		line = str(line)
		if line:
			for l in line.split('\n'):
				s.out.write('\t%s\n' % l)
	def GetText(s):
		return s.out.getvalue()

	def TestBegin(s):
		s.out.write('TESTCASE "%s %s";\n' % (GroupIndex(), s))
		if ps_2_x:
			flags = []
			if s.pred.value:
				flags.append('USEPREDICATE')
			if s.arbSwizzle:
				flags.append('USEARBSWZ')
			if flags:
				s.write(' '.join(flags))
		s.write('PASSBEGIN')
		if s.pred.value:
			s.write('PixelShaderConstantF[2] = {%s};' % ', '.join([`p` for p in s.pred.value]))
		if s.DeclareConstantsPerTestcase:
			for src in s.sources:
				dec = src.DeclareShaderConstant(s.decld).strip()
				if dec:
					s.write(dec)
		s.write('TESTBEGIN')
	def TestEnd(s):
		s.write('TESTEND')
	def ResultConst(s):
		s.write('def c1, %s' % ', '.join([str(r) for r in s.maskedresult]))
	def SourceConst(s):
		for src in s.sources:
			dec = src.Declare(s.decld).strip()
			if dec:
				s.write(dec)
	def InitSourceReg(s):
		for n,src in zip(range(len(s.sources)), s.sources):
			if src.reg[0] == 'r':
				dec = src.DeclareRReg().strip()
				if dec:
					s.write(dec)
##	def PredConst(s):
##		if s.pred.value:
##			s.write('def c2, %s' % ', '.join([`p` for p in s.pred.value]))
	def SetPred(s):
		if s.pred.value:
			s.write('SET_PRED')
	def ClearOutputReg(s):
		s.write('mov r0, c0.y')
	def SetEpsReg(s):
		s.write('mov r2, c0.%s' % (s.pp_eps and 'w' or 'z'))
	def WriteOp(s):
		s.write(s)
	def VerifyResult(s):
		s.write('VERIFY_RESULT')
		s.write(' '.join(['V_%s' % c for c in 'XYZW']))
	def WriteTest(s):
		s.TestBegin()
		s.ResultConst()
		s.SourceConst()
##		s.PredConst()
		s.SetPred()
		s.InitSourceReg()
		s.ClearOutputReg()
		s.SetEpsReg()
		s.WriteOp()
		s.VerifyResult()
		s.TestEnd()

class Operation:
	"Keeps track of op modifiers, Apply operation"
	def __init__(s, op, sat=0, pp=0):
		s.op = op
		s.sat = sat
		s.pp = pp
		s.desc = '%s'
		if sat: s.desc += '_sat'
		if pp: s.desc += '_pp'
	def __str__(s):
		return s.desc % s.op
	def Apply(s, result):
		"apply operation, in this case just saturate result"
		return Saturate(result, s.sat)

class Predication:
	"Keeps track of instruction predication"
	def __init__(s, reg='p0', value=0, pnot=0, pswz=0):
		s.reg = 'p0'
		if type(value) is str:
			value = [PredicateMap[c] for c in value]
		s.value = value
		s.pnot, s.pswz = pnot, pswz
		pnot = pnot and '!' or ''
		pswz = pswz and ('.'+s.pswz) or ''
		s.desc = '(%s%%s%s) ' % (pnot, pswz)
	def __str__(s):
		if(s.value):
			return s.desc % s.reg
		else:
			return ''
	def Transform(s):
		"Return value of predicate after modifiers have been applied"
		return ApplyPredMod(s.value, s.pnot, s.pswz)

class Parameter:
	def __init__(s, reg):
		"reg is the source or dest register without modifiers, ex: 'r1' or 'c0'"
		s.reg = reg
		s.desc = '%s'						# %s will be surrounded by parameter modifiers, swizzles, mask
	def __str__(s):
		return s.desc % s.reg

class Destination(Parameter):
	def __init__(s, reg, mask=0):
		apply(Parameter.__init__, (s, reg))
		s.mask = mask
		if s.mask:
			s.desc += '.' + s.mask

class Source(Parameter):
	def __init__(s, reg, value, neg=0, abs=0, swz=0):
		Parameter.__init__(s, reg)
		s.abs, s.neg, s.swz = abs, neg, swz
		s.value = value
		s.Default()
		s.mask = FillMask(swz)
		s.desc = DescribeSource(abs, neg, swz)
	def Transform(s):
		return ApplySrcMod(s.value, s.abs, s.neg, s.swz)
	def Default(s):
		if s.value is None:
			s.value = defaults[s.reg][:]
	def DeclareShaderConstant(s, decld):
		result = ''
		reg = s.reg
		if reg[0] in 'rcbi':
			if reg[0] == 'r':
				reg = 'c' + str(int(reg[1])+c_SourceStartIndex-r_SourceStartIndex)
			typ = 'F'
			if reg[0] in 'bi':
				typ = reg[0] == 'b' and 'B' or 'I'
			regnum = int(reg[1:])
			if reg not in decld:
				result = 'PixelShaderConstant%s[%d] = {%s};' % (typ, regnum, ', '.join(map(str, s.value)))
				decld[reg] = 1
		return result
	def Declare(s, decld):
		result = ''
		reg = s.reg
		if reg[0] in 'rcbi':
			if None:
				if reg[0] == 'r':
					reg = 'c' + str(int(reg[1])+c_SourceStartIndex-r_SourceStartIndex)
				if reg not in decld:
					result = 'def %s, %s' % (reg, ', '.join(map(str, s.value)))
					decld[reg] = 1
		else:
			if ps_ver < 3.0:
				if s.reg not in decld:
					result = 'dcl %s' % s.reg
					decld[s.reg] = 1
			else:
				if s.reg not in decld:
					result = 'dcl_texcoord%d %s' % (int(s.reg[1]), s.reg)
					decld[s.reg] = 1
		return result
	def DeclareRReg(s):
		return 'mov %s, c%d' % (s.reg, int(s.reg[1])+c_SourceStartIndex-r_SourceStartIndex)


class ScalarSource(Source):
	def __init__(s, reg, value, neg=0, abs=0, swz=0):
		Source.__init__(s, reg, [value]*4, abs, neg, swz)
	def Transform(s):
		return ApplySrcMod([s.value]*4, s.abs, s.neg, s.swz)[0]
	def DeclareShaderConstant(s, decld):
		src = s.value
		s.value = [src]*4
		result = Source.DeclareShaderConstant(s, decld)
		s.value = src
		return result
	def Declare(s, decld):
		src = s.value
		s.value = [src]*4
		result = Source.Declare(s, decld)
		s.value = src
		return result

class MatrixSource(Source):
	def __init__(s, reg, value, neg=0, abs=0, swz=0, rows=4, cols=4):
		assert (not neg) and (not abs) and (not swz), "can't specify neg,abs,swz for matrix source"
		if value is not None:
			value = [ v[:cols] for v in value[:rows] ]
		s.rows, s.cols = rows, cols
		Source.__init__(s, reg, value, abs, neg, swz)
	def Transform(s):
		"Returns copy of value, since modifiers are invalid for Matrix Source"
		return [ v[:] for v in s.value ]
	def Default(s):
		if s.value is None:
			s.value = [ defaults[s.reg[0] + str(int(s.reg[1])+r)][:s.cols]
						for r in range(s.rows) ]
	def DeclareShaderConstant(s, decld):
		result = ''
		# save actual value and starting register
		src = s.value
		reg = s.reg
		try:
			for r in range(s.rows):
				s.reg = reg[0] + str(int(reg[1])+r)
				s.value = src[r]  # *s.cols : (r+1)*s.cols ]
				s.value += [0 for n in range(4-s.cols)]
				result += Source.DeclareShaderConstant(s, decld) + '\n'
		finally:
			# restore actual value and starting register
			s.value = src
			s.reg = reg
		return result
	def Declare(s, decld):
		result = ''
		# save actual value and starting register
		src = s.value
		reg = s.reg
		try:
			for r in range(s.rows):
				s.reg = reg[0] + str(int(reg[1])+r)
				s.value = src[r]  # *s.cols : (r+1)*s.cols ]
				s.value += [0 for n in range(4-s.cols)]
				result += Source.Declare(s, decld) + '\n'
		finally:
			# restore actual value and starting register
			s.value = src
			s.reg = reg
		return result
	def DeclareRReg(s):
		result = []
		reg = s.reg
		for r in range(s.rows):
			s.reg = reg[0] + str(int(reg[1])+r)
			result.append(Source.DeclareRReg(s))
		s.reg = reg
		return '\n'.join(result)


def WriteVS(defaults, colors):
	"""Write vertex shader based on regular and color defaults for sources.
	defaults = list of 4-vectors for normal default values
	colors = list of 4-vectors for <= 2.x v# register default values"""
	c_start = 5		# first available vertex shader constant slot
	if ps_ver < 3.0:
		defs = defaults + colors
		movs = ['mov oT%d, c%d' % (n, c_start+n) for n in range(len(defaults))]
		movs += ['mov oD%d, c%d' % (n, c_start+len(defaults)+n) for n in range(len(colors))]
	else:
		defs = defaults
		dcls = ['dcl_texcoord%d o%d' % (n, n+1) for n in range(len(defs))]
		movs = ['mov o%d, c%d' % (n+1, c_start+n) for n in range(len(defs))]
	defs = ['def c%d, %s' % (c_start+n, ', '.join([str(c) for c in defs[n]]))
			for n in range(len(defs))]
	if ps_ver < 3.0:
		return ProcessBlock('vs_2_0_template', ['\n'.join(defs), '\n'.join(movs)] )
	else:
		return ProcessBlock('vs_3_0_template', ['\n'.join(dcls), '\n'.join(defs), '\n'.join(movs)] )


def WritePSConstantDefaults(defaults):
	out = []
	for n in range(len(defaults)):
		out.append('PixelShaderConstantF[%d] = {%s};' % (c_SourceStartIndex + n, ', '.join(['%s'%d for d in defaults[n]])))
	return '\n'.join(out)


# So we can group testcases and embed a group code in the test description string
def SetGroup(name):
	global curGroup, curGroupStart
	curGroup = name
	curGroupStart = curCount
def GroupIndex():
	if curGroup:	return '%s(%d)' % (curGroup, curCount - curGroupStart)
	else:	return ''
curCount = 0
curGroup = ''
curGroupStart = 0


# maps predicate chars to values:
PredicateMap = {'0':0, '1':1}

# make a mapping between swizzle chars and integer indices:
SwizzleMap = dict(zip('xyzw', range(4)) + zip('rgba', range(4)))

# Write Mask (16):
#	.x, .y, .z, .w,
#	.xy, .xz, .xw, .yz, .yw, .xw
#	.xyz, .xyw, .xzw, .yzw, .xyzw
lstWriteMasks = [ x+y+z+w for w in ['','w'] for z in ['','z'] for y in ['','y'] for x in ['','x'] ][1:]
lstBasicWriteMasks = 'x,y,z,w,xz,yw,xyw,xyzw'.split(',')

# Predicate mask:
# first we need a set of predicate register values:
lstAllPredicateValues = [x+y+z+w for x in '01' for y in '01' for z in '01' for w in '01']
lstSomePredicateValues = ['1010', '0101', '1111', '0000']

def SetupSwizzles():
	global lstReplicateSwizzles, lstSimpleSwizzles, lstArbitrarySwizzles
	global lstUsedSwizzles, lstRandomSwizzles
	global lstBasicSwz, lstSupportedSwz
	
	# Only Replicate Swizzles:
	lstReplicateSwizzles = ['x', 'y', 'z', 'w']

	# Simple Swizzles:
	lstSimpleSwizzles = 'xyzw,x,y,z,w,yzxw,zxyw,wzyx'.split(',')

	# Arbitrary Swizzles (21):
	lstArbitrarySwizzles = map(string.strip, """
		xy, xz, xw, yx, zx, zy, wx, wy, wz, 
		zyx, wzy, wzx, xwx, yww, yzx, xwy,
		xzyw, xyxw, xxyy, wxyx, zywz
		""".split(','))

	lstUsedSwizzles = lstSimpleSwizzles + lstArbitrarySwizzles

	# simplify swizzles in list of used swizzles:
	lstUsedSwizzles = map(SimplifySwizzle, lstUsedSwizzles)

	# Random Swizzles (20):
	# Make a list of 20 random swizzles, or until we give up at 10 tries for a unique swizzle:
	lstRandomSwizzles = []
	for n in range(20):
		s = RandSwizzle(10)
		if not s: break
		lstRandomSwizzles.append(s)

	lstBasicSwz = 'x,y,zxyw'.split(',')
	lstSupportedSwz = lstSimpleSwizzles[:]
	if use_arbitrary_swizzles:
		lstBasicSwz += 'xw,wzx,yzwy'.split(',')
		lstSupportedSwz += lstArbitrarySwizzles + lstRandomSwizzles


eps = ldexp(1, -10)			# spec says something about 21 bits of precision, so we'll set eps to 2^-15
eps_pp = ldexp(1, -5)		# spec says 10 bits for partial precision, so we'll set eps to 2^-7


try:
	SetVersion(ps_version)
except:
	pass





# Some possible source values for operations: (all divided by 32 or something)
##add = [-15, -57, 43, 63, 22, -8, -32, 39]
##m4x4 = [50, -46, 23, -47, 34, 24, 61, 59, 43, -61, 62, -52, -41, 10, -37, -58, -57, -17, -63, 55]
##mad = [46, -57, 62, 13, -58, -63, -18, 52, 39, 63, -38, -5]


