You will find in this page detailed instructions to install PVSnesLib latest release on Windows or Linux (ubuntu or other debian distribution) environment.

Before continuing, please keep in mind that PVsneslib works only on **32 bits** system for now. To avoid conflicts between versions, we recommend to install required dependencies in 32 bits when it is applicable (for msys and gcc).

To follow this guide, you will need :

- the [latest release](https://github.com/alekmaul/pvsneslib/releases/latest) of PVsneslib

and these required dependencies

- msys _if you are on Windows environment_
- GCC for some tools
- python 3 to optimize code
- a text editor to write your code
- an emulator to test the roms

All other required dependencies to use it are provided in the archive to download.
If you want to go further and build the latest version from sources, you can follow [this page](https://github.com/alekmaul/pvsneslib/wiki/Compiling-from-sources) which explain how to do it.

As PVSneslib works on different operating system, it is hard to cover all cases, specifically on Linux which have a lot of differences.
As you have probably a package management tool installed, it is more easy to do it from command line and i will provide some instructions using the **apt-get** tool which download packages from APT repository.

**Feel free to adapt it for your version !**


### Required dependencies

#### MSYS (step required for Windows only)

PVsneslib need application to run scripts and makefiles like you do on Linux environments.
If you work on Linux, please ignore this step but on Windows, you will need a tool to do it : MSYS.
You can download it from [portabledev.com](https://www.portabledev.com/wp-content/files/msys-1.0.17.exe) (the official Alekmaul’s website, developer of the library) or you can find newest version easily on internet.

You can install it everywhere on your computer but the path need to be added in the **PATH** environment variable to works correctly with PVSneslib.

Other tools as **git bash** or **cygwin** are not compatible for now.

#### GCC

Depending on the tools already installed on your computer, you may need to install GCC too. For example, you will have to install it if you encounter this error building projects which use smconv tool (like the mario-like sample) :

![smConv error](https://user-images.githubusercontent.com/981773/120016823-5e926300-bfe5-11eb-9ec3-c76223072ae0.png)

It is also necessary if you want to build yourself some tools in devkitsnes (like smconv !).

##### On Windows

You can download the latest version of the tool [here](https://sourceforge.net/projects/tdm-gcc/) but we recommend you to get the 32 bit one, as we advised you for other tools!

##### On Linux

As explained before, i will show the command line using apt-get tool.
By running this command line, you will have gcc :
`sudo apt install gcc`

For gfx2snes, you will need to install gcc-multilib too :
`sudo apt install gcc-multilib`

#### Python 3

##### On Windows

Download and install the latest version of [python 3](https://www.python.org/downloads/windows/) compatible with your OS version.

##### On Linux

You just need to run :
`sudo apt install python-is-python3`

##### Check that it works!

Python is used to optimize the code produced, it needs to be added in your path and accessible directly from the python command. If the command `python --version` does not work on your computer, you will have issues while using the script in PVsneslib.

Please note that the code is still compatible with Python 2 but we do not recommend to use this old version.


#### Text editor

Any text editor will allow you to write your code but you will need to use a terminal beside to execute the `make` commands to build your project.

For convenience reasons, we recommend to use Visual Studio Code which is cross platform. To do it, you can follow this [Wiki page](https://github.com/alekmaul/pvsneslib/wiki/PVSneslib-and-Visual-Studio-Code).

#### Emulator

A lot of emulators for the Super Nintendo are available and the behaviour of your code can be different depending the one you use.
If you are on Windows, we directly recommend to use [no$snes](https://problemkaputt.de/sns.htm) which contains usefull functionalities.

Otherwise, you can use [Mesen-S](https://www.mesen.ca/) which is a high-accuracy emulator for Windows and Linux.

You can use any other emulator you want but in all cases, it is important to test you rom on different one which wich works as nearest possible than the real hardware.

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
`export PVSNESLIB_HOME=/path/to/pvsneslib`

But if you do not want to execute this command before using the library, you can add it to the file **.profile** located in your home directory.

Open the terminal and execute this commands after editing the path with your own:

`cd && echo export PVSNESLIB_HOME=/path/to/pvsneslib >> .profile`

It will add this variable to your .profile file, you now need to reload it to take this change into account.

To do it, just launch `source ~/.profile` command in your terminal.

If you want to use the **~** character to point on your home directory, you can also do it like that : `export PVSNESLIB_HOME=~/pvsneslib`

### It is ready !


Now everything is installed correctly, you can validate that it works correctly by compiling the hello world sample. Just go under your installation path, snes-examples > hello_world and run the `make` command.


If you encounter an issue, join us on [Discord](https://discord.gg/DzEFnhB) to get help!
