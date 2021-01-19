Pulsar
================
[![CI](https://github.com/p-sam/switch-libpulsar/workflows/CI/badge.svg?branch=develop)](https://github.com/p-sam/switch-libpulsar/actions?query=workflow%3ACI)

Switch homebrew library to load, parse, and play sounds from BFSAR files and related file formats

This library is in development and may not implement complete parsing of each format it supports. Details not needed for sound browsing and playback on the Switch may be left unimplemented.

## Links

* [Github repository](https://github.com/p-sam/switch-libpulsar)
* [Online documentation](https://p-sam.github.io/switch-libpulsar/)

## Including in your project

See [Setting up Development Environment](https://switchbrew.org/wiki/Setting_up_Development_Environment) to install the required build dependencies

There are multiple ways to bundle the code as part of your project which are:

### Building the library

Build with `make` and link against `pulsar` in your project.
Then add the `include` and `lib` dirs to your corresponding search paths
(If you're using a standard homebrew Makefile, this is as easy as adding this directory to the `LIBDIRS` variable).

### Include the library sources in your project

Include `deps.mk` in your Makefile, and then use the imported variables to add this library sources to your project (just like the Makefile at the root of this repo does).

* `PLSR_SOURCES`: Absolute paths to source folders
* `PLSR_INCLUDES`: Absolute path to the include folder

## Additional resources

### Building examples

Build with `make examples` at the root of this repo, or with `make` directly in the `examples` folder.
Binaries will be located at `./examples/build/*.nro`

### Building the documentation

[Doxygen](https://www.doxygen.nl) is required to generate documentation from source code.

Build with `make docs` at the root of this repo, or with `make` directly in the `docs` folder.
The resulting documentation will be located at `./docs/build/html`.

If you find yourself editing the documentation often, use `make docs-watch` in the `docs` folder to regenerate the documentation when the source files change.
