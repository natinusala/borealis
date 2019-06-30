dep_glfw3 = dependency('glfw3', version : '>=3.3')
dep_glm   = dependency('glm', version : '>=0.9.8')

borealis_files = files(
    'lib/glad.c',
    'lib/nanovg/nanovg.c',

    'lib/Application.cpp',
    'lib/View.cpp',
    'lib/SettingsFrame.cpp',
    'lib/Theme.cpp',
    'lib/TabFrame.cpp',
    'lib/Rectangle.cpp',
    'lib/BoxLayout.cpp',
    'lib/Sidebar.cpp',
    'lib/Animations.cpp',
    'lib/Style.cpp',
    'lib/List.cpp',
    'lib/Label.cpp',
    'lib/CrashFrame.cpp',
    'lib/Button.cpp',
    'lib/TableView.cpp',
    'lib/SelectView.cpp',

    'lib/libretro-common/compat/compat_strl.c',
    'lib/libretro-common/features/features_cpu.c',
    'lib/libretro-common/encodings/encoding_utf.c'
)

borealis_include = include_directories('include', 'include/nanovg', 'include/libretro-common')

borealis_dependencies = [ dep_glfw3, dep_glm ]