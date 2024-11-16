import os

# build options

def str_to_bool(string):
	if(string=='yes' or string=='y' or string=='1' or string=='true' or string=='on'):
		return True
	elif(string=='no' or string=='n' or string=='0' or string=='false' or string=='off'):
		return False
	else:
		return None

def get_bool_option(name, default):
	if(name in ARGUMENTS):
		out = str_to_bool(ARGUMENTS[name])
		if(out==None):
			print("invalid value for '"+name+"': "+ARGUMENTS[name])
			sys.exit(1)
		return out
	else:
		return default

debug = get_bool_option('debug',False)
optimize = get_bool_option('optimize',False)

# initial environment
env = Environment(
	CPPPATH=['src/', 'glad/include/', 'glfw/include'],
	CXXFLAGS=['-std=c++20'],
	LIBS=['png'])

if(debug):
	env.Append(CXXFLAGS=['-g','-O0'])
if(optimize):
	env.Append(CXXFLAGS=['-O3'])

# export compile_commands.json
env.Tool('compilation_db')
env.CompilationDatabase()

# set sources
source_files = Glob('src/*.cpp',exclude='src/main.cpp') + Glob('src/core/*.cpp') + Glob('src/defs/*.cpp') + Glob('src/util/*.cpp') + Glob('glad/src/glad.c')

if('test' in ARGUMENTS):
	test = ARGUMENTS['test']
	if(os.path.isdir('tests/'+test)):
		source_files += Glob('tests/'+test+'/*.cpp')
	else:
		print("Unknown test: "+test)
		sys.exit(1)
else:
	source_files += Glob('src/main.cpp')

# build glfw if it's not there
if(not os.path.isfile('glfw-3.4/build/src/libglfw3.a')):
	os.system('cmake -S glfw-3.4 -B glfw-3.4/build')
	os.system('cmake --build glfw-3.4/build')
if(not os.path.isfile('glfw-3.4/build/src/libglfw3.a')):
	print("failed to build glfw")
	sys.exit(1)

static_libs = Glob('glfw-3.4/build/src/libglfw3.a')

env.Program('Motor',source_files+static_libs)
