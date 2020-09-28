import os, sys, re
from StringIO import StringIO

def Icmp(a, b):
	return cmp(a.lower(), b.lower())
def esc(s):
	return s.replace('\\', '\\\\')

def WriteResources(out, path, localDir, group = None):
	if group:
		relPath = os.path.join(localDir, group)
	else:
		relPath = localDir
	files = os.listdir(path)
	files.sort(Icmp)
	for f in files:
		name, ext = os.path.splitext(f)
		if ext.lower() != '.fx':
			continue
		if os.path.isfile(os.path.join(path, f)):
			out.write('%s RCDATA ".\\\\%s\\\\%s"\n' % (f, esc(relPath), f))

def AddResources(rcName, targetDir, onlyGroup = None):
	reIdentifier = re.compile('[a-z_][a-z_0-9]*', re.I)
	reBegin = re.compile(r'^//\[\[\[BeginFXResources', re.M)
	reEnd = re.compile(r'^//EndFXResources\]\]\]', re.M)
	rootDir, localDir = os.path.split(targetDir)
	if not localDir:	# takes care of trailing backslash
		localDir = os.path.split(rootDir)[1]
	rc = file(rcName, 'rt').read()
	mBegin = reBegin.search(rc)
	assert mBegin, 'Must have //[[[BeginFXResources ... //EndFXResources]]] block'
	mEnd = reEnd.search(rc, mBegin.end())
	assert mEnd, 'Must have //[[[BeginFXResources ... //EndFXResources]]] block, end missing'
	rcFile = StringIO()
	rcFile.write(rc[:mBegin.end()])
	rcFile.write('\t// Following lines are generated, do not edit:\n')
	if onlyGroup == '.':
		WriteResources(rcFile, targetDir, localDir, None)
	else:
		files = os.listdir(targetDir)
		files.sort(Icmp)
		for f in files:
			if onlyGroup and f.lower() != onlyGroup.lower():
				continue
			m = reIdentifier.match(f)
			path = os.path.join(targetDir, f)
			if not os.path.isdir(path):
				continue
			if not m or m.group(0) != f:
				print 'Group dirs must follow identifier rules, "%s" does not.' % f
				continue
			rcFile.write('//---- Begin Group %s ----\n' % f)
			WriteResources(rcFile, path, localDir, f)
	rcFile.write(rc[mEnd.start():])
	file(rcName, 'wt').write(rcFile.getvalue())

def AddTests(mainName, targetDir):
	assert 0, "do not use... old function"
	reBegin = re.compile(r'^//\[\[\[BeginAutoTests', re.M)
	reEnd = re.compile(r'^//EndAutoTests\]\]\]', re.M)
	reMeta = re.compile(r'(.+)_Meta\.fx', re.I)
	reInclude = re.compile(r'Test_Include\.fx', re.I)
	localDir = os.path.split(targetDir)[1]
	main = file(mainName, 'rt').read()
	mBegin = reBegin.search(main)
	assert mBegin, 'Must have //[[[BeginAutoTests ... //EndAutoTests]]] block'
	mEnd = reEnd.search(main, mBegin.end())
	assert mEnd, 'Must have //[[[BeginAutoTests ... //EndAutoTests]]] block, end missing'
	mainFile = StringIO()
	mainFile.write(main[:mBegin.end()])
	mainFile.write('\t// Following lines are generated, do not edit:\n')
	files = os.listdir(targetDir)
	for f in files:
		name, ext = os.path.splitext(f)
		if ext.lower() != '.fx':
			continue
		if reInclude.match(f) or reMeta.match(f):
			continue	# skip Test_Include.fx and _Meta files
		if os.path.isfile(os.path.join(targetDir, f)):
			mainFile.write('CCompiler TEST_%s( "%s" );\n' % (name, name))
	mainFile.write(main[mEnd.start():])
	file(mainName, 'wt').write(mainFile.getvalue())

def Usage():
	print """Usage:
	AddResources [-g<only-group>] [<alt-root>]
	AddResources -h     (for this message)
	"""
	sys.exit()

if __name__ == '__main__':
	from getopt import getopt
	try:
		opts, args = getopt(sys.argv[1:], 'g:h')
	except:
		Usage()
	targetDir = 'TestCases'
	onlyGroup = None
	if len(args):
		if len(args) > 1:
			Usage()
		targetDir = args[0]
	for o,a in opts:
		if 'g' in o.lower():
			onlyGroup = a
		if 'h' in o.lower():
			Usage()

	AddResources('ShaderValidator.rc', targetDir, onlyGroup)
	#AddTests('main.cpp', targetDir)

