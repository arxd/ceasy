# Main SCons file
from os.path import join, exists

debug = int(ARGUMENTS.get('debug',3))

env = Environment(tools=['default',TOOL_HDRTMPL, TOOL_SHADER])
if debug <= 1:
	env.Append(CCFLAGS = '-O3')
else:
	env.Append(CCFLAGS = '-g')
env.Append(CCFLAGS = '-DLOG_LEVEL=%d'%debug)

for demo in map(str, Glob('demos/*')):
	SConscript(join(demo, 'SConstruct'), variant_dir=join('build',demo), exports='env' )

shared = SConscript('src/shared/SConscript', variant_dir='build/src/shared', exports='env')

for device in map(str, Glob('src/*')) :
	if device == 'src/shared':
		continue
	SConscript(join(device,'SConstruct'), variant_dir=join('build',device), exports='env shared' )

Clean('.', 'build')
Clean('.', 'lib')
Clean('.', 'bin')
Clean('.', 'include')


