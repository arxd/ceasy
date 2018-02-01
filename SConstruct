
DEBUG = 0

env = Environment()

for demo in Glob('demos/*'):
	SConscript('%s/SConstruct'%demo, variant_dir='build/%s'%demo)
SConscript('src/pixie/SConstruct', variant_dir='build/pixie')
