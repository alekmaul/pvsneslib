If you want to build the whole pvsneslib project directly from sources, you will need some informations.

_Before following this tutorial, we strongly recommend you to do a first installation from the release version thanks to [this page.](https://github.com/alekmaul/pvsneslib/wiki/Installation)_
_It will provide you tools and command lines to install dependencies and avoid you some loss of time here !_

Please keep in mind that it works only on **32 bits** system. If your system is not able to manage it, you can use docker by following instructions available in the docker folder.
I will also consider that these tools are mandatory everywhere so i will not add it in the list anymore :

  * git
  * msys
  * GCC

## Clone the repository

By using the `git` command, clone the repository first :
`git clone --recurse-submodules --depth=1 https://github.com/alekmaul/pvsneslib.git`

The `recurse-submodules` option will download **WLA** and **TCC** tools which are required and stored in other repositories.

`depth=1` is optional but allow you to download only the last version of source code. If you do not want all modifications history, you will probably win some time and disk space by using it.

## Create PVSNESLIB_HOME

To avoid forgetting it later, we can begin from this point. Just create an environment variable **PVSNESLIB_HOME**
pointing on your pvsneslib directory.

For example : /c/pvsneslib

Be careful: the path must be in Unix style (/c/ instead of c:\\) on Windows too !
Do not forget that if you change your environment variable, you will probably need to launch a new command prompt
to take it into account.

Please remember that **PVSNESLIB_HOME** variable (or any folder you will create under its tree) **CANNOT contains spaces**.

## Build the tools

To build the tools and because you already installed msys and GCC, you will not need other dependencies!

You can go the the tools folder in the root of repository folder, then run `make` command.

When everything is built correctly, you can run `make install` to move all tools under devkitsnes directory.

## Build the compilers

### TCC

To install tcc, you will need :

 * texi2html to build the documentation (optional)

In the compiler folder, you can run the `make tcc` command. It will run the configuration script (to generate configurations files).
Once finished, you can run `make tcc-install` to move it to devkitsnes directory.

If you did not installed texi2html, the step to generate the documentation will be ignored.

### WLA

WLA is a git submodule of pvsneslib. If you cloned the PVSneslib repository from command line without the `--recurse-submodules` parameter, you probably have to download the submodule with `git submodule init && git submodule update` command.

To install WLA you will need :

* Cmake

In the compiler folder, you can run the `make wla` command.
It will run cmake command and make on the 3 parts of WLA we need to develop on SNES (wla-65816, wla-spc700 and wlalink)

Once finished, you can run `make wla-install` to move it to devkitsnes directory.


### TCC and WLA at the same time

If you have installed dependencies, you can directly run the `make` command from compiler directory then `make install` to move it to devkitsnes directory.
It will build tcc then wla.


## Build PVSneslib

To install PVSneslib, you will need :

 * doxygen for documentation (optional)

To build PVSneslib, **PVSNESLIB_HOME** environment variable have to be defined. If you followed all steps of this page, you should arleady have it.

Tools and compiler should be compiled and installed before too !

Go to pvsneslib directory and run the `make` command. There is no the `make install` because it generate automatically the files in the final directory.

If you did not installed Doxygen, the step to generate the documentation will be ignored.


## Build sample

If everything works correctly until here, samples will be really easy to build.

Just run the `make` command from snes-examples directory then `make install` to move all the rom files generated to the bin folder.

## Use the debug mode

During the build process, PVSneslib apply some changes on the .asm files automatically generated from .c sources.

It can be useful to see the result of each step to investigate on an issue or to debug it.  
To keep these temporary files without modifying scripts, just define the **PVSNESLIB_DEBUG** environment variable to **1** by doing `export PVSNESLIB_DEBUG=1` before compiling.

You will see some `*.xx.dbg` files created where `xx` is the step number in the process.

## For the next time

Now all dependencies are installed and works correctly, you can go quicker to build the whole project.

Execute `make` command in the root folder and all dependencies will be compiled and installed in the right folder.

****