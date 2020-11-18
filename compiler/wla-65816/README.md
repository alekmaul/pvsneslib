wla-dx
======

WLA DX - Yet Another
GB-Z80/Z80/6502/65C02/65CE02/6510/65816/6800/6801/6809/8008/8080/HUC6280/SPC-700 
Multi Platform Cross Assembler Package

This is just a link to compile it for WIN32 platform

Installing
==========

Compiling
---------

You need:

* CMake (at least v2.8.9)
* C compiler
* Sphinx (optional, min. v1.2.0, v1.6.0 recommended) (to generate docs)

1. `git clone https://github.com/vhelin/wla-dx # Clone the repository`
2. `cd wla-dx`
3. `mkdir build && cd build # Create building directory`
4. `cmake -G "MSYS Makefiles" .. # Generate build system for W32`
5. `cmake --build . --config Release # Build it`
6. `cmake -P cmake_install.cmake # Install it (optional)`

If you didn't installed it, you can still access the binaries in the `binaries`
folder. It also contains the compiled documentation in the subfolder `doc`.

You can add `-DCMAKE_INSTALL_PREFIX=prefix` to Steps 4 and/or 6 (before the `-P`)
to set the prefix directory it'll install in.


Usage
=====

Use the [documentation](https://wla-dx.readthedocs.io/en/latest/) as a starting
point.

