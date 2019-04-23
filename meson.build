
project('hybrid_app', ['c', 'cpp'],
	version: '1.0.0',
	default_options: [ 'buildtype=release', 'strip=true', 'b_ndebug=if-release', 'cpp_std=c++14' ],
)

dep_glfw3 = dependency('glfw3', version : '>=3.3', static : true)
dep_glm   = dependency('glm', version : '>=0.9.8')

hybrid_app_files = files(
	'source/main.cpp',
	'source/lenny.c',
	'source/glad.c',
)

hybrid_app = executable(
	'hybrid_app',
	hybrid_app_files,
	dependencies : [ dep_glfw3, dep_glm ],
	install: true,
)
