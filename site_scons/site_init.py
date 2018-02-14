import re, os.path
from subprocess import Popen, PIPE

def htmpl_extract(node):
	return re.findall(r'^@include\s+(\S+)', node.get_text_contents(), re.M)

def htmpl_scanner(node, env, path):
	#~ includes = 
	dir = str(node.get_dir())
	#~ print("SCAN: %s > %s"%(node, includes))
	return env.File([os.path.join(dir,i) for i in htmpl_extract(node)])

def htmpl_emitter(target, source, env):
	#~ source += re.compile(r'^@include\s+(\S+)$', re.M).findall(source[0].get_text_contents())
	includes = htmpl_extract(source[0])
	#~ print("EMIT: %s > %r"%(source[0], map(str, includes)))
	return (target, source + includes)
	
def htmpl_action(target, source, env):
	#~ print("BUILD: %s"%source[0])
	src = open(str(source[0]))
	dest = open(str(target[0]), 'w')
	for line in src:
		if line.startswith("@"):
			with open(os.path.join(str(source[0].get_dir()), line[8:].strip())) as inc:
				dest.write(inc.read())
		else:
			dest.write(line)
	return None



def shader_compile(target, source, env):
	prog = ['./site_scons/ccglsl', str(source[0]), str(target[0])]
	proc = Popen(prog, stdout=PIPE, stderr=PIPE )
	out,err = proc.communicate()
	print(out)
	return err
	
def shader_emitter(target, source, env):
	source.append('#site_scons/ccglsl')
	return (target, source)


def TOOL_SHADER(env):
	if Platform().name == 'win32':
		libs = [ 'glfw3dll', 'opengl32']
	else:
		libs = ['GLESv2', 'glfw']

	ccglsl = env.Program('#site_scons/ccglsl', ["#site_scons/shader_compiler.c"] , LIBS=libs)
	#~ shaders = env.Command('shaders.h', 'shaders.glsl', shader_compile)
	#~ env.Depends(shaders, ccglsl)
	env.Append(BUILDERS = {'GLSL' : Builder(
		action = shader_compile,
		suffix = '.h',
		src_suffix = '.glsl',
		#~ source_scanner =  Scanner(function = htmpl_file_scan, skeys = ['.in.h']),
		emitter = shader_emitter)})

def TOOL_HDRTMPL(env):
	env.Append(BUILDERS = {'HeaderTmpl' : Builder(
		action = htmpl_action,
		suffix = '.h',
		src_suffix = '.h.in',
		source_scanner =  Scanner(function = htmpl_scanner, skeys = ['.in.h']),
		emitter = htmpl_emitter)})