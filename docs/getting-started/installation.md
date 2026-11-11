# Installation

PVSnesLib is distributed as a ready-to-use archive for Windows, Linux and macOS. The archive contains the main development tools needed to build the examples. The setup differs slightly by operating system.

For the full, maintained installation walkthrough, consult the [official Installation Wiki page](https://github.com/alekmaul/pvsneslib/wiki/Installation). The steps below summarize the current release-based workflow.

## Before you begin

You will need:

- the [latest PVSnesLib release](https://github.com/alekmaul/pvsneslib/releases/latest);
- a text editor;
- a Super Nintendo emulator to test the ROMs you build.

Keep the PVSnesLib installation path free of spaces. The `PVSNESLIB_HOME` environment variable must point to the extracted PVSnesLib directory.

## 1. Download and extract PVSnesLib

Download the release archive for your operating system from the [GitHub Releases page](https://github.com/alekmaul/pvsneslib/releases/latest), then extract it to a directory of your choice.

For example, on Windows you can use `C:/snesdev/pvsneslib`. Avoid paths containing spaces.

## 2. Configure the environment

### Windows

PVSnesLib uses a Unix-like shell environment to run its scripts and Makefiles. Install [MSYS2](https://www.msys2.org/) and use its **UCRT64** terminal.

In the MSYS2 UCRT64 terminal, update the environment and install `make` as described in the [official Wiki instructions](https://github.com/alekmaul/pvsneslib/wiki/Installation). The Wiki currently gives these commands:

```sh
pacman -Suuy
pacman -S make
```

Restart MSYS2 after the update.

Set `PVSNESLIB_HOME` to the PVSnesLib installation directory, using MSYS-style path syntax. For an installation in `C:/snesdev/pvsneslib`, the value is `/c/snesdev/pvsneslib`:

```bat
setx PVSNESLIB_HOME "/c/snesdev/pvsneslib"
```

Open a new terminal after changing the environment variable. Always build projects from the MSYS2 UCRT64 terminal.

### Linux

Set `PVSNESLIB_HOME` to the directory where you extracted PVSnesLib. For the current terminal session:

```sh
export PVSNESLIB_HOME=/path/to/pvsneslib
```

To make the setting persistent, add the export to your shell profile (for example, `~/.profile`) and reload that profile.

### macOS

From the PVSnesLib installation directory, set the variable for the current terminal session:

```sh
export PVSNESLIB_HOME="$(pwd)"
```

The Wiki installation guide also specifies GNU sed. Install it with Homebrew:

```sh
brew install gnu-sed
```

Add Homebrew's GNU sed `gnubin` directory to your `PATH` as described in the [macOS section of the official Wiki guide](https://github.com/alekmaul/pvsneslib/wiki/Installation), then reload your shell configuration.

> **Important:** The Wiki notes that the installation path (and folders under it) must not contain spaces. On macOS, the `PVSNESLIB_HOME` command above applies to the current terminal session; configure your shell profile if you want it to persist.

## 3. Verify the installation

Open a terminal appropriate for your operating system, go to the `snes-examples/hello_world` directory inside your PVSnesLib installation, and run:

```sh
make
```

If the setup is correct, the example should build and produce a SNES ROM. Open that ROM in your preferred emulator to test it.

## 4. Continue learning

- Follow [First project](first-project.md) to understand the structure of a PVSnesLib project.
- Browse the [tutorials](../tutorials/index.md) for graphics, sprites, input and audio.
- Use the [API reference](../api/index.md) when you need function and header details.

## Troubleshooting and updates

Installation details can change as releases and operating systems evolve. If a command does not match your environment, use the [official Installation Wiki page](https://github.com/alekmaul/pvsneslib/wiki/Installation) as the current reference and report outdated instructions through the [GitHub issue tracker](https://github.com/alekmaul/pvsneslib/issues).
