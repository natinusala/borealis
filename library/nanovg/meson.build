dep_glfw3 = dependency('glfw3', version : '>=3.3')
dep_glm   = dependency('glm', version : '>=0.9.8')

nanovg_files = files(
    'source/nanovg.c',
    'source/pc/glad.c',
    'source/pc/wrapper.cpp',
)

nanovg_include = include_directories(
    'include',
)

nanovg_dependencies = [ dep_glfw3, dep_glm ]
