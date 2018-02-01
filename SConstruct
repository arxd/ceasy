
DEBUG = 0

env = Environment(CFLAGS="-g -DEBUG")

for demo in Glob('demos/*'):
	SConscript('%s/SConstruct'%demo, variant_dir='build/%s'%demo, exports='env' )
SConscript('src/pixie/SConstruct', variant_dir='build/pixie', exports='env')

Clean('.', 'build')
Clean('.', 'lib')
Clean('.', 'bin')
Clean('.', 'include')
