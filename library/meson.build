dep_glfw3 = dependency('glfw3', version : '>=3.3')
dep_glm   = dependency('glm', version : '>=0.9.8')

borealis_files = files(
    'lib/glad.c',
    'lib/nanovg/nanovg.c',

    'lib/Application.cpp',
    'lib/View.cpp',
    'lib/AppletFrame.cpp',
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
    'lib/Table.cpp',
    'lib/Dropdown.cpp',
    'lib/Logger.cpp',
    'lib/StagedAppletFrame.cpp',
    'lib/ProgressDisplay.cpp',
    'lib/ProgressSpinner.cpp',
    'lib/Image.cpp',
    'lib/Header.cpp',
    'lib/PopupFrame.cpp',

    'lib/TaskManager.cpp',
    'lib/RepeatingTask.cpp',

    'lib/SwkbdUtils.cpp',

    'lib/libretro-common/compat/compat_strl.c',
    'lib/libretro-common/features/features_cpu.c',
    'lib/libretro-common/encodings/encoding_utf.c'
)

borealis_include = include_directories('include', 'include/nanovg', 'include/libretro-common')

borealis_dependencies = [ dep_glfw3, dep_glm ]
