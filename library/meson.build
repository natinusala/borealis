dep_glfw3 = dependency('glfw3', version : '>=3.3')
dep_glm   = dependency('glm', version : '>=0.9.8')

borealis_files = files(
    'lib/extern/glad/glad.c',
    'lib/extern/nanovg/nanovg.c',
    'lib/extern/fmt/src/format.cc',
    'lib/extern/fmt/src/os.cc',

    'lib/extern/libretro-common/compat/compat_strl.c',
    'lib/extern/libretro-common/features/features_cpu.c',
    'lib/extern/libretro-common/encodings/encoding_utf.c',

    'lib/application.cpp',
    'lib/view.cpp',
    'lib/applet_frame.cpp',
    'lib/theme.cpp',
    'lib/tab_frame.cpp',
    'lib/rectangle.cpp',
    'lib/box_layout.cpp',
    'lib/sidebar.cpp',
    'lib/animations.cpp',
    'lib/style.cpp',
    'lib/list.cpp',
    'lib/label.cpp',
    'lib/crash_frame.cpp',
    'lib/button.cpp',
    'lib/table.cpp',
    'lib/dropdown.cpp',
    'lib/logger.cpp',
    'lib/staged_applet_frame.cpp',
    'lib/progress_display.cpp',
    'lib/progress_spinner.cpp',
    'lib/image.cpp',
    'lib/header.cpp',
    'lib/popup_frame.cpp',
    'lib/thumbnail_frame.cpp',
    'lib/layer_view.cpp',
    'lib/dialog.cpp',
    'lib/material_icon.cpp',
    'lib/hint.cpp',
    'lib/scroll_view.cpp',
    'lib/absolute_layout.cpp',

    'lib/task_manager.cpp',
    'lib/notification_manager.cpp',

    'lib/repeating_task.cpp',

    'lib/swkbd.cpp',

    'lib/i18n.cpp',
)

borealis_include = include_directories(
    'include',
    'lib/extern/fmt/include',
    'include/borealis/extern',
)

borealis_dependencies = [ dep_glfw3, dep_glm ]
