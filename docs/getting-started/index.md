# Getting started

Welcome to PVSnesLib! This section takes you from preparing your development environment to building and running your first Super Nintendo ROM.

You do not need to understand every detail of the SNES hardware before starting. Follow the steps in order, then use the tutorials and examples as you add features to your project.

## Your first steps

<div class="grid cards" markdown>

-   :material-download: **1. Install the toolchain**

    ---

    Set up PVSnesLib and the required development tools.

    [Installation guide](installation.md)

-   :material-console: **2. Build your first ROM**

    ---

    Compile the supplied `hello_world` example and run the resulting ROM.

    [First project](first-project.md)

-   :material-book-open-variant: **3. Explore the tutorials**

    ---

    Learn how to add backgrounds, sprites, controller input, and audio.

    [Tutorial overview](../tutorials/index.md)

-   :material-code-braces: **4. Study the examples**

    ---

    Browse working projects and learn from their source code.

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
| Getting started | Install PVSnesLib and build your first ROM. |
| Tutorials | Learn a practical development technique. |
| API reference | Look up functions, types, constants, and declarations in Doxygen. |
| SNES hardware | Understand the console resources and hardware constraints. |
| Tools | Learn the role of the asset converters and build utilities. |
| Examples | Find source projects that demonstrate specific features. |
| FAQ | Find answers to common documentation and project questions. |
| Community Wiki | Explore community tips, troubleshooting, and historical material. |

## After your first build

Once `hello_world.sfc` runs successfully, make a copy of the example and begin changing one feature at a time.

A useful learning path is:

1. Display a background using [Graphics and backgrounds](../tutorials/graphics.md).
2. Add an object using [Sprites](../tutorials/sprites.md).
3. Read the controller using [Controller input](../tutorials/input.md).
4. Add music or sound effects using [Audio](../tutorials/audio.md).
5. Compare your work with the projects in [Examples](../examples/index.md).

For function signatures and complete API details, use the [generated Doxygen reference](https://alekmaul.github.io/pvsneslib/). The [GitHub Wiki](https://github.com/alekmaul/pvsneslib/wiki) remains a complementary community resource.
