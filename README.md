# Borealis

Nintendo Switch inspired UI library, for PC and Nintendo Switch. Powered by nanovg. Example project based on the [hybrid glfw app](https://github.com/fincs/hybrid_app) by fincs.

## Building the example for Switch

To build for Switch, a standard development environment must first be set up. In order to do so, [refer to the Getting Started guide](https://devkitpro.org/wiki/Getting_Started).

```bash
(sudo) (dkp-)pacman -S switch-glfw switch-mesa switch-glm
make -j
nxlink -s hybrid_app.nro
```

## Building the example for PC

To build for PC, the following components are required:

- meson/ninja build system
- A C++ compiler supporting the C++14 standard
- GLFW version 3.3 or higher (as a static library)
- GLM version 0.9.8 or higher

Please refer to the usual sources of information for your particular operating system. Usually the commands needed to build this project will look like this:

```bash
meson build
ninja -C build
./build/borealis
```

Also, please note that the `resources` folder must be available in the working directory, otherwise the program will fail to find the shaders.

### Building the example for Windows using msys2

msys2 provides all packages needed to build this project:

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-meson mingw-w64-x86_64-ninja mingw-w64-x86_64-pkg-config mingw-w64-x86_64-glfw mingw-w64-x86_64-glm
meson build
ninja -C build
./build/borealis
```
