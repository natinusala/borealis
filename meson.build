
project('borealis', ['c', 'cpp'],
    version: '1.0.0',
    default_options: [ 'buildtype=release', 'strip=true', 'b_ndebug=if-release', 'cpp_std=c++14' ],
)

dep_glfw3 = dependency('glfw3', version : '>=3.3')
dep_glm   = dependency('glm', version : '>=0.9.8')

example_files = files(
    'example/main.cpp'
)

libretro_common_files = files(
    'library/libretro-common/compat/compat_strl.c',
    'library/libretro-common/features/features_cpu.c',
    'library/libretro-common/encodings/encoding_utf.c'
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
    'library/Sidebar.cpp',
    'library/Animations.cpp'
)

borealis_example = executable(
    'borealis_example',
    [ example_files, library_files, libretro_common_files ],
    dependencies : [ dep_glfw3, dep_glm ],
    install: true,
    include_directories: include_directories('include', 'include/nanovg', 'include/libretro-common'),
    cpp_args: [ '-DWINDOW_NAME="Borealis example"', '-g', '-O2' ]
)
