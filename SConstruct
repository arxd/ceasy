# Main SCons file

debug = int(ARGUMENTS.get('debug',3))

env = Environment(tools=['default',TOOL_HDRTMPL, TOOL_SHADER])
if debug <= 1:
	env.Append(CCFLAGS = '-O3')
else:
	env.Append(CCFLAGS = '-g')
env.Append(CCFLAGS = '-DLOG_LEVEL=%d'%debug)

for demo in Glob('demos/*'):
	if demo not in ['demos/hello', 'demos/pong']:
		continue
	SConscript('%s/SConstruct'%demo, variant_dir='build/%s'%demo, exports='env' )

#~ shared_utils = SConscript('src/shared/SConstruct', variant_dir='build/src/shared', exports='env')


for device in Glob('src/*') :
	if str(device) not in ['src/pixie']:
		continue
	print("SCONS: %s"%device)
	SConscript('%s/SConstruct'%device, variant_dir='build/%s'%device, exports='env' )

Clean('.', 'build')
Clean('.', 'lib')
Clean('.', 'bin')
Clean('.', 'include')


