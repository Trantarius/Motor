

# initial environment
env = Environment(
	CPPPATH=['src/', 'glad/include/', 'glfw/include'],
	CXXFLAGS=['-std=c++20'],
	LIBS=['png'])

# export compile_commands.json
env.Tool('compilation_db')
env.CompilationDatabase()

# set sources
source_files = Glob('src/*.cpp') + Glob('src/core/*.cpp') + Glob('src/defs/*.cpp') + Glob('src/util/*.cpp') + Glob('glad/src/glad.c')

# build glfw if it's not there
if(!os.path.isfile('glfw-3.4/build/src/libglfw3.a')):
	os.system('cmake -S glfw-3.4 -B glfw-3.4/build')
	os.system('cmake --build glfw-3.4/build')
if(!os.path.isfile('glfw-3.4/build/src/libglfw3.a')):
	sys.exit(1)

static_libs = Glob('glfw-3.4/build/src/libglfw3.a')

env.Program('Motor',source_files+static_libs)
