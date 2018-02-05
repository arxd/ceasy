# Main SCons file

DEBUG = 0

env = Environment(CFLAGS="-g -DEBUG", tools=['default',TOOL_HDRTMPL, TOOL_SHADER])

# for demo in Glob('demos/*'):
	# SConscript('%s/SConstruct'%demo, variant_dir='build/%s'%demo, exports='env' )

shared_utils = SConscript('src/shared/SConstruct', variant_dir='build/src/shared', exports='env')


# for device in Glob('src/*') :
	# if str(device) == 'src/shared':
		# continue
	# print("SCONS: %s"%device)
	# SConscript('%s/SConstruct'%device, variant_dir='build/%s'%device, exports='env shared_utils' )

Clean('.', 'build')
Clean('.', 'lib')
Clean('.', 'bin')
Clean('.', 'include')


