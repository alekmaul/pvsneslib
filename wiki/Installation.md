You will find in this page detailed instructions to install PVSnesLib latest release on Windows or Linux (ubuntu or other debian distribution) or MacOS environment.

To follow this guide, you will need :

- the [latest release](https://github.com/alekmaul/pvsneslib/releases/latest) of PVsneslib

and these required dependencies

- msys2 _if you are on Windows environment_
- a text editor to write your code
- an emulator to test the roms

All other required dependencies to use it are provided in the archive to download.
If you want to go further and build the latest version from sources, you can follow [this page](https://github.com/alekmaul/pvsneslib/wiki/Compiling-from-sources) which explain how to do it.

As PVSneslib works on different operating system, it is hard to cover all cases, specifically on Linux which have a lot of differences.
As you have probably a package management tool installed, it is more easy to do it from command line and i will provide some instructions using the **apt-get** tool which download packages from APT repository.

**Feel free to adapt it for your version !**


### Required dependencies

#### MSYS2 (step required for Windows only)

PVsneslib need application to run scripts and makefiles like you do on Linux environments.
If you work on Linux, please ignore this step but on Windows, you will need a tool to do it : MSYS2 UCRT64

You can download it from [here](https://www.msys2.org/#installation). Just follow the instructions from step `1` to `5`.

When `MSYS2` is installed, we want first to be sure that all is updated.
Then, we have to install the `make` tool to compile all samples available.

In the `MSYS2 UCRT64` terminal, run:

- `pacman -Suuy` to update the rest of the packages (allowing downgrades)
- `pacman -S make` to install the `make` command

You **MUST** restart MSYS2.

Please remember that:

- Always use **MSYS2 UCRT64** terminal windows to compile your code.


#### Text editor

Any text editor will allow you to write your code but you will need to use a terminal beside to execute the `make` commands to build your project.

For convenience reasons, we recommend to use Visual Studio Code which is cross platform. To do it, you can follow this [Wiki page](https://github.com/alekmaul/pvsneslib/wiki/PVSneslib-and-Visual-Studio-Code).

#### Emulator

A lot of emulators for the Super Nintendo SNES are available and the behaviour of your code can be different depending the one you use.
If you are on Windows, we directly recommend to use [no$snes](https://problemkaputt.de/sns.htm) which contains useful functionalities.

Otherwise, you can use [Mesen](https://www.mesen.ca/) which is a high-accuracy emulator for Windows and Linux.

You can use any other emulator you want but in all cases, it is important to test you rom on different one which which works as nearest possible than the real hardware.

### Installing PVSnesLib

From the version 3.5.0 (May 2022), PVsneslib is now packaged in one zip to ease the installation.

The first thing you need is to download the latest version of PVsneslib then extract it where you want.
Then, you need to create a new environment variable **PVSNESLIB_HOME** to provide the path to this directory.

Please remember that **PVSNESLIB_HOME** variable (or any folder you will create under its tree) **CANNOT contains spaces**.

#### On windows

You could extract it under `C:/snesdev` directory.
You can set **PVSNESLIB_HOME** to `/c/snesdev` by using this command line: `setx PVSNESLIB_HOME "/c/snesdev"`, or you can set it manually like it is shown below:

![PVSNESLIB_HOME environment variable on Windows](https://user-images.githubusercontent.com/48180545/170839914-f5f21198-2df9-4190-ae72-ee459c4bc790.png)

**Be careful**: the path must be in Unix style (`/c/` instead of `c:\`) on Windows too ! Do not forget that if you change your environment variable, you will probably need to launch a new command prompt to take it into account.


#### On Linux

Depending your distribution, you have different ways to create this variable.

If you want to declare the variable only for the current terminal session, you can execute :
```bash
export PVSNESLIB_HOME=/path/to/pvsneslib
```

But if you do not want to execute this command before using the library, you can add it to the file **.profile** located in your home directory.

Open the terminal and execute this commands after editing the path with your own:

```bash
cd && echo export PVSNESLIB_HOME=/path/to/pvsneslib >> .profile
```

It will add this variable to your .profile file, you now need to reload it to take this change into account.

To do it, just launch `source ~/.profile` command in your terminal.

If you want to use the **~** character to point on your home directory, you can also do it like that : `export PVSNESLIB_HOME=~/pvsneslib`

#### On MacOS

You can accomplish it by using the following command in your terminal in your pvsneslib directory:

```bash
export PVSNESLIB_HOME=$(pwd)
```

Please remember that:

- `PVSNESLIB_HOME` environment variable (or any folder you will create under its tree) **CANNOT contains spaces**.
- This environment variable is not persistent. It means you'll have to set it if you launch a new terminal, or close the current one.

Don't worry, if you forget, you will be notified of an error message by running the `make` command.


Then, install [*brew*](https://brew.sh/):

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

Finally, install brew package:

```bash
brew install gnu-sed
```

For `gnu-sed`, we need to set it as your default sed command. Let's do it by updating the `PATH` environment.

A quick way to do this is by adding PATH at the end of your `~/.zshrc`

Edit this file and add this the end, or update the existing environment variable `PATH` if it already exists by using your favorite file editor *(`nano`, `vim`, ...)*.

```bash
# Some people mention that the path is now "/opt/homebrew/opt/gnu-sed/libexec/gnubin", please confirm us!
PATH="/usr/local/opt/gnu-sed/libexec/gnubin:$PATH"
export PATH
```
Save the file and reload it:

```
source ~/.zshrc
```

Now `gnu-sed` will be used by default because it precedes all other sed commands installed on your system.

### It is ready !


Now everything is installed correctly, you can validate that it works correctly by compiling the hello world sample. Just go under your installation path, snes-examples > hello_world and run the `make` command.


If you encounter an issue, join us on [Discord](https://discord.gg/DzEFnhB) to get help!
