# Compiling the project from source

> This wiki explains how to install the development tools on your system (see below the list of available systems).

_Before following this tutorial, we strongly recommend you to do a first installation from the release version thanks to [this page.](https://github.com/alekmaul/pvsneslib/wiki/Installation)_

_It will provide you tools and command lines to install dependencies and avoid you some loss of time here!_

_It means that this tuturial is **only** needed if you wish to participate to the project itself and make your own contribution_

- [Compiling the project from source](#compiling-the-project-from-source)
  - [Installing the development tools](#installing-the-development-tools)
    - [Installing the development tools on Windows.](#installing-the-development-tools-on-windows)
    - [Installing the development tools on Linux.](#installing-the-development-tools-on-linux)
      - [Installing the development tools on Ubuntu.](#installing-the-development-tools-on-ubuntu)
      - [Installing the development tools on Debian.](#installing-the-development-tools-on-debian)
      - [Installing the development tools on Fedora.](#installing-the-development-tools-on-fedora)
      - [Installing the development tools on Centos.](#installing-the-development-tools-on-centos)
    - [Installing the development tools on MacOS.](#installing-the-development-tools-on-macos)
  - [Compiling the project](#compiling-the-project)
    - [Clone the repository](#clone-the-repository)
    - [Set the `PVSNESLIB_HOME` environment variable](#set-the-pvsneslib_home-environment-variable)
    - [Build the whole project](#build-the-whole-project)
    - [Build the whole project and create your own release](#build-the-whole-project-and-create-your-own-release)


## Installing the development tools


### Installing the development tools on Windows.

First at all, you have to install MSYS2 as provided in the installation documentation [here](https://www.msys2.org/#installation). Just follow the instructions from step `1` to `5`.

When `MSYS2` is installed, we want first to be sure that all is updated.

In the `MSYS2` terminal, update it:

- Run `pacman -Suuy` to update the rest of the packages (allowing downgrades). You **MUST** restart MSYS2.

Now we can install the development tools needed to compile **PVSNESLIB**.

In the **MSYS2** terminal, install the following packages:

```bash
pacman -Sy mingw-w64-ucrt-x86_64-toolchain
pacman -Sy mingw-w64-ucrt-x86_64-cmake
pacman -Sy mingw-w64-ucrt-x86_64-doxygen
pacman -Sy mingw-w64-ucrt-x86_64-pcre2 
pacman -Sy base-devel
pacman -Sy git
pacman -Sy zip
```

Congratulations, you're done to start to compile **PVSNESLIB** on *Windows* !

Now, to use the MSYS2 UCRT64 toolchain, just open the MSYS2 UCRT64 terminal. See [here](https://github.com/alekmaul/pvsneslib/blob/b2682bc7610c6cab1158746ac482e334655a0b7a/wiki/PVSneslib-and-Visual-Studio-Code.md) to integrate it under Visual Studio Code.


### Installing the development tools on Linux.

#### Installing the development tools on Ubuntu.

> This procedure explains how to install the development tools on *Ubuntu* latest *(22)*.

First at all, you have to ensure that your distribution is up to date.

In a terminal:

```bash
sudo apt update -y
```
Once it's done, in case of change, you should **reboot** your system.

Then, you can install the development tools:

```bash
sudo apt-get install -y build-essential \
    gcc-12 \
    cmake \
    make \
    git \
    doxygen \
    texi2html \
    texlive-latex-base \
    texlive-latex-recommended \
    texlive-latex-extra
```

Finally, set `gcc-12` as your default compiler:

```bash
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 90
```
> This step will set `gcc-12` as your the default compiler. the `90` is the priority, here higher priority. We need to do it because the default compiler on Ubuntu 22 is `clang`.

Congratulations, you're done to start to compile **PVSNESLIB** on *Ubuntu*!

#### Installing the development tools on Debian.

> This procedure explains how to install the development tools on *debian* latest *(11)*.

First at all, we need to add the `testing` component from the **Debian** repository, which contains `gcc-12`.

```bash
echo 'deb http://deb.debian.org/debian testing main' >> /etc/apt/sources.list
```

Then, let's ensure that your distribution is up to date.

In a terminal:

```bash
sudo apt update -y
```
Once it's done, in case of change, you should **reboot** your system.

Then, you can install the development tools:

```bash
sudo apt-get install -y build-essential \
    gcc-12 \
    cmake \
    make \
    git \
    doxygen \
    texi2html \
    texlive-latex-base \
    texlive-latex-recommended \
    texlive-latex-extra
```

Finally, set `gcc-12` as your default compiler:

```bash
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 90
```
> This step will set `gcc-12` as your the default compiler. the `90` is the priority, here higher priority. We need to do it because the default compiler on Debian 11 is `gcc-11`.

Congratulations, you're done to start to compile **PVSNESLIB** on *Debian*!

#### Installing the development tools on Fedora.

> This procedure explains how to install the development tools on *Fedora* latest *(37)*.

First at all, you have to ensure that your distribution is up to date.

In a terminal:

```bash
sudo dnf update -y
```
Once it's done, in case of change, you should **reboot** your system.

Then, you can install the development tools:

```bash
sudo dnf -y groupinstall "Development Tools"
sudo dnf -y install cmake \
    gcc-c++ \
    glibc-static \
    libstdc++-static \
    git \
    doxygen \
    texi2html \
    texlive-scheme-basic
```
Congratulations, you're done to start to compile **PVSNESLIB** on *Fedora*!

#### Installing the development tools on Centos.

> This procedure explains how to install the development tools on *Centos* latest *(7)*.

First at all, you have to ensure that your distribution is up to date and that the `powertools` repository is enable on your system.

In a terminal:

```bash
sudo dnf -y install dnf-plugins-core
sudo dnf -y config-manager --set-enabled powertools
sudo dnf update -y
```
Once it's done, in case of change, you should **reboot** your system.

Then, you can install the development tools:

```bash
sudo dnf -y groupinstall "Development Tools"
sudo dnf -y install cmake \
        gcc-c++ \
        glibc-static \
        libstdc++-static \
        git \
        doxygen \
        texi2html \
        texlive-scheme-basic
```
Congratulations, you're done to start to compile **PVSNESLIB** on *Centos*!

### Installing the development tools on MacOS.

> This procedure explains how to install the development tools on *MacOS*.

First at all, you have to ensure that your distribution is up to date.

In a terminal:

```zsh
sudo softwareupdate -i -a
```

> Note: You can also use the graphical interface to perform the same action. It's really up to you.

Then, install [*Xcode*](https://developer.apple.com/xcode/).

```bash
Xcode-select —install
```

Then, install [*brew*](https://brew.sh/):

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

Finaly, install brew packages:

```bash
brew install doxygen
brew install gnu-sed
brew install texi2html
brew install texlive
```

For the same reason that Ubuntu (clang being installed as the default compiler) We need to create symlinks to set `gcc` and `g++` to respectively `gcc-12` and `g++-12`:

```bash
sudo ln -s /usr/local/bin/gcc-12 /usr/local/bin/gcc
sudo ln -s /usr/local/bin/g++-12 /usr/local/bin/g++
```
Ditto for `gnu-sed`, we need to set it as your default sed command. Let's do it by updating the `PATH` environment.

A quick way to do this is by adding PATH at the end of your `~/.zshrc`

Edit this file and add this the end, or update the existing environment variable `PATH` if it already exists by using your favorite file editor *(`nano`, `vim`, ...)*.

```bash
PATH="/usr/local/opt/gnu-sed/libexec/gnubin:$PATH"
export PATH
```
Save the file and reload it:

```
source ~/.zshrc
```

Now `gnu-sed` will be used by default because it preceeds all other sed commands installed on your system.

Congratulations, you're done to start to compile **PVSNESLIB** on *MacOS*!

## Compiling the project

> This procedure is supposed to work whatever the system used: *Windows under MSYS2*, *Linux* or *MacOS*.


### Clone the repository

By using the `git` command, clone the repository first :

```bash
git clone --recurse-submodules --depth=1 https://github.com/alekmaul/pvsneslib.git
```

The `recurse-submodules` option will download **WLA** and **TCC** tools which are required and stored in other repositories.

`depth=1` is optional but allow you to download only the last version of source code. If you do not want all modifications history, you will probably win some time and disk space by using it.

### Set the `PVSNESLIB_HOME` environment variable

To avoid forgetting it later, we can begin from this point. Just create an environment variable **PVSNESLIB_HOME**
pointing on your `pvsneslib` directory.

You can accomplish it by using the following command in your terminal:

```bash
export PVSNESLIB_HOME=$(pwd)
```

Please remember that:

- `PVSNESLIB_HOME` environment variable (or any folder you will create under its tree) **CANNOT contains spaces**.
- This environment variable is not persistent. It means you'll have to set it if you launch a new terminal, or close the current one.

Don't worry, if you forget, you will be notified of an error message by running the `make` command.


### Build the whole project

Building the whole project can be performed by simply running the `make` command at the root of the project.

> Note: The Makefile at the root of the project is written to run the all the others Makefile in the subdirectories. It will always perform a full clean recipe before to compile as well as the install recipe after compilation.

Once the compilation is done, you can find all the tools, compilers and libraries in the `devkitsnes` directory.

### Build the whole project and create your own release

This procedure is pretty the same as building the whole project, just add the `release` after your make command:

```bash
make release
```

Once the compilation is done, you can find your compressed release *(zip)* in the `release` directory.
