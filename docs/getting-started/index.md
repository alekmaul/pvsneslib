# Getting started

PVSnesLib lets you create homebrew games and applications for the Super Nintendo **in the C programming language**. This section takes you from preparing the development environment to building and running your first ROM.

You write the game's application logic in C and call PVSnesLib functions to work with SNES features such as backgrounds, sprites, controller input and audio. The supplied toolchain builds the project into a ROM for the console. Some low-level library and platform components use 65816 assembly; you do not need to write assembly to follow the first-project guide.

## Your first steps

<div class="grid cards" markdown>

-   :material-download: **1. Install the toolchain**

    ---

    Set up PVSnesLib and the required development tools.

    [Installation guide](installation.md)

-   :material-console: **2. Build your first C project**

    ---

    Compile the supplied `hello_world` C example and run the resulting ROM.

    [First project](first-project.md)

-   :material-book-open-variant: **3. Explore the tutorials**

    ---

    Learn how to add backgrounds, sprites, controller input, and audio to your C project.

    [Tutorial overview](../tutorials/index.md)

-   :material-code-braces: **4. Study the examples**

    ---

    Browse working C projects and learn from their source code.

    [Examples](../examples/index.md)

</div>

## What you need

Before building a project, make sure that:

- PVSnesLib is installed and its environment variable is configured.
- You are using the terminal and build environment specified by the installation instructions.
- The supplied examples are available in your PVSnesLib installation.
- You have a SNES emulator available to test the generated `.sfc` ROM.

For platform-specific setup and version notes, follow the [Installation guide](installation.md).

## Understand the documentation

The documentation is organized by task:

| Section | Use it when you want to… |
| --- | --- |
| Getting started | Install PVSnesLib and build your first C project and ROM. |
| Tutorials | Learn a practical technique for your game. |
| API reference | Look up functions, types, constants, and declarations in Doxygen. |
| SNES hardware | Understand the console resources and hardware constraints. |
| Tools | Learn the role of the asset converters and build utilities. |
| Examples | Find C source projects that demonstrate specific features. |
| FAQ | Find answers to common documentation and project questions. |
| Community Wiki | Explore community tips, troubleshooting, and historical material. |

## After your first build

Once `hello_world.sfc` runs successfully, make a copy of the example and begin changing one feature at a time.

A useful learning path is:

1. Display a background using [Graphics and backgrounds](../tutorials/graphics.md).
2. Add an object using [Sprites](../tutorials/sprites.md).
3. Read the controller using [Controller input](../tutorials/input.md).
4. Add music or sound effects using [Audio](../tutorials/audio.md).
5. Compare your work with the C projects in [Examples](../examples/index.md).

For function signatures and complete API details, use the [generated Doxygen reference](../api/index.md). The [GitHub Wiki](https://github.com/alekmaul/pvsneslib/wiki) remains a complementary community resource.
