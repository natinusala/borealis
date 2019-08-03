project('borealis_example', ['c', 'cpp'],
    version: '1.0.0',
    default_options: [ 'buildtype=release', 'strip=true', 'b_ndebug=if-release', 'cpp_std=c++1z' ],
)

add_global_arguments('-DWINDOW_NAME="Borealis example"', language : 'cpp')

subdir('library')

example_files = files(
    'example/main.cpp',
    'example/SampleInstallerPage.cpp',
    'example/SampleLoadingPage.cpp'
)

borealis_example = executable(
    'borealis_example',
    [ example_files, borealis_files ],
    dependencies : borealis_dependencies,
    install: true,
    include_directories: [ borealis_include, include_directories('example')],
    cpp_args: [ '-g', '-O2', '-DBOREALIS_RESOURCES="./resources/"' ]
)
