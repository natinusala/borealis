
project('borealis', ['c', 'cpp'],
    version: '1.0.0',
    default_options: [ 'buildtype=release', 'strip=true', 'b_ndebug=if-release', 'cpp_std=c++14' ],
)

dep_glfw3 = dependency('glfw3', version : '>=3.3')
dep_glm   = dependency('glm', version : '>=0.9.8')

example_files = files(
    'example/main.cpp'
)

library_files = files(
    'library/glad.c',
    'library/nanovg/nanovg.c',

    'library/Application.cpp',
    'library/View.cpp',
    'library/SettingsFrame.cpp',
    'library/Theme.cpp',
    'library/TabFrame.cpp',
    'library/Rectangle.cpp',
    'library/BoxLayout.cpp',
    'library/Sidebar.cpp'
)

borealis_example = executable(
    'borealis_example',
    [ example_files, library_files ],
    dependencies : [ dep_glfw3, dep_glm ],
    install: true,
    include_directories: include_directories('include', 'include/nanovg'),
    cpp_args: [ '-DWINDOW_NAME="Borealis example"', '-g' ]
)
