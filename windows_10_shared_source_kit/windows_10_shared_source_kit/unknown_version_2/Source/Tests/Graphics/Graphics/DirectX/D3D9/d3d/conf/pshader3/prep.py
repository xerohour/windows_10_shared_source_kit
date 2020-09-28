files = """
	inst_setp.fx
	inst_rcp.fx
	inst_rsq.fx
	inst_dp2add.fx
	inst_mNxN.fx
	inst_crs.fx
	inst_dpN.fx
	inst_lrp.fx
	inst_mad.fx
	inst_nrm.fx
	inst_exp.fx
	inst_log.fx
	inst_pow.fx
	inst_sincos.fx
	""".split()

if __name__ == '__main__':
	import PrePy
	from os import path, system
	import sys
	from getopt import getopt

	# Process command line args:
	opts,args = getopt(sys.argv[1:], 'ce')

	sd_check, sd_edit = False, False
	for o,v in opts:
		if o == '-c':
			sd_check = True
		elif o == '-e':
			sd_edit = True

	if args:
		files = args

	ver_count = 0
	file_count = 0
	for ver in ['2.0', '2.x', '3.0']:
		ver_ = ver.replace('.', '_')
		config = "ps_version = %s\n" % `ver`
		for f in files:
			# skip setp for ps_2_0, since it doesn't exist:
			if f == 'inst_setp.fx' and ver == '2.0':
				continue
			
			try:
				outfile = path.join('ps'+ver, 'ps%s_%s' % (ver_, f))
				if sd_check:
					system('sd opened "%s"' % outfile)	# will print yellow if target not opened
				elif sd_edit:
					system('sd edit "%s"' % outfile)	# will open the file for edit
				else:
					print outfile
				f = f + '.prep'
				try:
					i = file(f, 'r')
				except IOError:
					print sys.exc_info()[0], sys.exc_info()[1]
					continue	# skip this file
				try:
					o = file(outfile, 'w')
				except IOError:
					print sys.exc_info()[0], sys.exc_info()[1]
					continue	# skip this file
				PrePy.Initialize(config)
				o.write(PrePy.process(i.read(), f))
				i.close()
				o.close()
				file_count += 1
			except:		# if another error occurs just skip this file
				print 'Error Processing File "%s"' % outfile
				sys.excepthook(*sys.exc_info())
		ver_count += 1
	print '%d files generated for %d ps versions' % (file_count, ver_count)
