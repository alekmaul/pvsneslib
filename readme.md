# PVSnesLib

**P**rogrammer **V**aluable **Snes** **Lib**rary — a small, open, and free development kit for coding Nintendo SNES games in C (or assembly).

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/license/mit/)
[![Discord](https://img.shields.io/badge/chat-on%20discord-blue.svg)](https://discord.gg/DzEFnhB)
[![Current Release](https://img.shields.io/github/v/release/alekmaul/pvsneslib?label=Current%20Release)](https://github.com/alekmaul/pvsneslib/releases/latest)
[![Downloads](https://img.shields.io/github/downloads/alekmaul/pvsneslib/total?label=Total%20Downloads)](https://github.com/alekmaul/pvsneslib/releases)
[![Build](https://github.com/alekmaul/pvsneslib/actions/workflows/pvsneslib_build_package.yml/badge.svg?branch=develop)](https://github.com/alekmaul/pvsneslib/actions/workflows/pvsneslib_build_package.yml)

PVSnesLib bundles a compiler/linker toolchain and a library (with full source) that gives you ready-made functions for backgrounds, sprites, controller input, and music/sound on real SNES hardware — plus example projects that show how it all fits together. 

It was started for the SNES's 20th anniversary in 2012, building on Ulrich Hecht's earlier SNES [SDK](http://code.google.com/p/snes-sdk/), and has since grown into a mostly assembler-optimized library that's just as usable directly from asm if you'd rather skip C entirely.  

## Table of contents

- [Features](#features)
- [Quickstart](#quickstart)
- [Before you start](#before-you-start)
- [Installation](#installation)
- [Building from source / Docker](#building-from-source--docker)
- [Editor integration](#editor-integration)
- [Documentation](#documentation)
- [Contributing](#contributing)
- [Made with PVSnesLib](#made-with-pvsneslib)
- [Support the project](#support-the-project)
- [Special thanks](#special-thanks)

## Features

- **Backgrounds** — tile and map loading, multiple BG modes
- **Sprites** — OAM management helpers for animated sprites
- **Input** — controller/pad and mouse/superscope reading
- **Sound & music** — SPC700 audio driver integration
- **Toolchain included** — C compiler (816-tcc) and linker, no separate install needed
- **C or assembly** — write your whole game in C, drop into asm where you need the speed, or skip C altogether
- **HiROM / FastROM support**
- **Cross-platform** — prebuilt releases for Windows, Linux, and macOS
- **Example projects** included, covering common techniques (including a Mode 7 demo and a full map/platformer engine example)

## Quickstart

Get a project compiling before you dive into the details.

1. **Download** the release for your OS from [the latest release page](https://github.com/alekmaul/pvsneslib/releases/latest), and unzip it.
2. **Set the environment variable** the toolchain expects (adjust the path to where you unzipped it):

   ```bash
   export PVSNESLIB_HOME="/path/to/pvsneslib"
   ```

3. **Write the classic Hello World**, following [this tutorial](https://github.com/alekmaul/pvsneslib/wiki/Compiling-helloworld-example).


For the full walkthrough — Makefile setup, folder layout, and flashing to real hardware — see the [Wiki](https://github.com/alekmaul/pvsneslib/wiki).

## Before you start

PVSnesLib assumes you're **already comfortable with C**. Trying to learn C and SNES programming at the same time will slow you down a lot — get comfortable with C first. A **solid grasp of the SNES hardware** itself also goes a long way. Good starting points:

- [SFC Development Wiki](https://wiki.superfamicom.org/)
- [SNESdev Wiki](https://snes.nesdev.org/wiki/SNESdev_Wiki)
- [Super NES Programming (Wikibooks)](https://en.wikibooks.org/wiki/Super_NES_Programming/)

## Installation

Prebuilt releases are available for:

[![Windows](https://img.shields.io/badge/Windows-download-blue?logo=windows)](https://github.com/alekmaul/pvsneslib/releases/download/4.6.0/pvsneslib_460_64b_windows_release.zip)
[![Linux](https://img.shields.io/badge/Linux-download-orange?logo=linux)](https://github.com/alekmaul/pvsneslib/releases/download/4.6.0/pvsneslib_460_64b_linux_release.zip)
[![macOS](https://img.shields.io/badge/macOS-download-lightgrey?logo=apple)](https://github.com/alekmaul/pvsneslib/releases/download/4.6.0/pvsneslib_460_64b_darwin_release.zip)

Download the archive for your platform, unzip it, and follow the setup steps on the [Wiki](https://github.com/alekmaul/pvsneslib/wiki).  

Generated API documentation is also available [here](https://alekmaul.github.io/pvsneslib/).

## Building from source / Docker

Prebuilt releases not working for your setup? You can build PVSnesLib from source.

- On Windows, you'll need a unix-like environment such as **msys2**.
- A ready-to-use **Docker image** is provided — see the [docker](https://github.com/alekmaul/pvsneslib/tree/master/docker)  folder for build instructions.
- Full dependency list and build steps: [Compiling from sources (Wiki)](https://github.com/alekmaul/pvsneslib/wiki/Compiling-from-sources)

PVSnesLib supports Windows, Linux, and macOS.

## Editor integration

A Visual Studio Code template is available in [vscode-template](https://github.com/alekmaul/pvsneslib/tree/master/vscode-template) to get syntax highlighting, build tasks, and debugging set up quickly.

## Documentation

- [API documentation](https://alekmaul.github.io/pvsneslib/) (generated from source)
- [Project Wiki](https://github.com/alekmaul/pvsneslib/wiki) — setup, build details, and guides
- [Discord community](https://discord.gg/DzEFnhB) — ask questions, share your project

## Contributing

Bug reports, pull requests, and discussion are all welcome on the [GitHub project](https://github.com/alekmaul/pvsneslib) or on [Discord](https://discord.gg/DzEFnhB).

PVSnesLib and its affiliated tools are distributed under the MIT license — see [`pvsneslib_license`](https://github.com/alekmaul/pvsneslib/blob/master/pvsneslib/pvsneslib_license.txt).

## Made with PVSnesLib

A few of the games built on PVSnesLib *(non-exhaustive)*:

|  |  |  |
|:---:|:---:|:---:|
|![Yo-Yo Shuriken](https://github.com/alekmaul/pvsneslib/raw/master/pvsneslib/docs/images/yoyoshuriken.gif) |![Eyra](https://github.com/alekmaul/pvsneslib/raw/master/pvsneslib/docs/images/eyra.gif) |![Sydney Hunter](https://github.com/alekmaul/pvsneslib/raw/master/pvsneslib/docs/images/sydneycod.gif) |
| **Yo-Yo Shuriken** — [Dr. Ludos](https://drludos.itch.io/yo-yo-shuriken) | **Eyra** — [Second Dimension](https://www.second-dimension.com/store/eyra-the-crow-maiden-snes) | **Sydney Hunter** — [CollectorVision](https://collectorvision.com/store/shop/snes/snes-sydney-hunter-the-caverns-of-death-pal/) |

## Support the project

PVSnesLib is free to use. If it's been useful to you and help you create a game, consider supporting development.   

[![Donate](https://img.shields.io/badge/Donate-PayPal-yellow?logo=paypal)](https://paypal.me/GIRARD996)

## Special thanks

PVSnesLib exists thanks to a long list of contributors and tool authors — from toolchain maintainers and optimizers to the artists behind the intro logo and everyone active on [Discord](https://discord.gg/DzEFnhB). 

See the project's commit history and Discord community for the full cast of people who've helped it grow.

Thanks!
