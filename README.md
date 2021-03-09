# borealis

Controller and TV oriented UI library for PC and Nintendo Switch (libnx).

WIP - See the Projects tab to follow the journey towards a stable version!

The code for the old version is available in the `legacy` branch.

- Mimicks the Nintendo Switch system UI, but can also be used to make anything else painlessly
- Hardware acceleration and vector graphics with automatic scaling for TV usage (powered by nanovg)
- Can be ported to new platforms and graphics APIs by providing a nanovg implementation
- Powerful layout engine using flex box as a base for everything (powered by Yoga Layout)
- Automated navigation paths for out-of-the-box controller navigation
- Out of the box touch support
- Define user interfaces using XML and only write code when it matters
- Use and restyle built-in components or make your own from scratch
- Display large amount of data efficiently using recycling lists
- Integrated internationalization and storage systems
- Integrated toolbox (logger, animations, timers, background tasks...)

Documentation is available [right here](https://github.com/natinusala/borealis/wiki). As every documentation, it may not be up to date. Feel free to report any missing info or inconsistency you may find.

## Building the demo for Switch

To build for Switch, a standard development environment must first be set up. In order to do so, [refer to the Getting Started guide](https://devkitpro.org/wiki/Getting_Started).

```bash
(sudo) (dkp-)pacman -S switch-glfw switch-mesa switch-glm
make -j
nxlink -s borealis_demo.nro
```

## Building the demo for PC

To build for PC, the following components are required:

- meson/ninja build system
- A C++ compiler supporting the C++17 standard
- GLFW version 3.3 or higher (as a static library)
- GLM version 0.9.8 or higher

Please refer to the usual sources of information for your particular operating system. Usually the commands needed to build this project will look like this:

```bash
meson build
ninja -C build
./build/borealis_demo
```

Also, please note that the `resources` folder must be available in the working directory, otherwise the program will fail to find the shaders.

### Building the demo for Windows using msys2

msys2 provides all packages needed to build this project:

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-meson mingw-w64-x86_64-ninja mingw-w64-x86_64-pkg-config mingw-w64-x86_64-glfw mingw-w64-x86_64-glm
meson build
ninja -C build
./build/borealis_demo
```

### Including in your project (TL;DR: see the demo makefile in this repo)
0. Your project must be built as C++17 (`-std=c++1z`). You also need to remove `-fno-rtti` and `-fno-exceptions` if you have them
1. Use a submodule (or even better, a [subrepo](https://github.com/ingydotnet/git-subrepo)) to clone this repository in your project
2. Copy the `resources` folder to the root of your project
3. For PC (meson):
    1. take a standard meson file
    2. use `subdir` to import the library folder
    3. use the `borealis_files`, `borealis_dependencies`, `borealis_include` and `borealis_cpp_args` variables for respectively objects to build, dependencies (glfw...), includes directory and cpp args
    4. add a `BRLS_RESOURCES` define pointing to the resources folder at runtime (so `resources`)
4. For Switch:
    1. take a standard deko3d homebrew makefile (from the switch-examples repo)
    2. add a `BOREALIS_PATH` variable containing the subfolder you put the library in
    3. set `ROMFS` to the resources folder - borealis resources path is set to `romfs:/` in borealis.mk
    4. use `include` to load `borealis.mk` (after `LIBDIRS` and `BOREALIS_PATH`)
    5. it's important for `OUT_SHADERS` to be set to `shaders` since nanovg will look for the shaders there
    6. if you have issues with the `BRLS_RESOURCES` define (missing quote terminator), change the `@$(MAKE)` line to `@MSYS2_ARG_CONV_EXCL="-D;$(MSYS2_ARG_CONV_EXCL)" $(MAKE)` in the Makefile (as in the demo Makefile)
