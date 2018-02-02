# Main SCons file

DEBUG = 0

env = Environment(CFLAGS="-g -DEBUG", tools=['default',TOOL_HDRTMPL, TOOL_SHADER])

for demo in Glob('demos/*'):
	SConscript('%s/SConstruct'%demo, variant_dir='build/%s'%demo, exports='env' )

shared_utils = SConscript('src/shared/SConstruct', variant_dir='build/src/shared', exports='env')
print(shared_utils)

SConscript('src/pixie/SConstruct', variant_dir='build/src/pixie', exports='env')

#~ for device in [x for x in Glob('src/*') if x != 'src/shared']:
	#~ SConscript('%s/SConstruct'%device, variant_dir='build/%s'%device, exports='env shared_utils' )

#~ SConscript('src/pixie/SConstruct', variant_dir='build/pixie', exports='env')
#~ SConscript('src/pixie/SConstruct', variant_dir='build/pixie', exports='env')

Clean('.', 'build')
Clean('.', 'lib')
Clean('.', 'bin')
Clean('.', 'include')


