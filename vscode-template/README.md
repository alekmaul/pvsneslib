# Setting up PVSneslib with Visual Studio Code

This page is a short guide to configure Visual Studio Code with PVsneslib.
For more detailed instructions, please follow [this wiki page](https://github.com/alekmaul/pvsneslib/wiki/PVSneslib-and-Visual-Studio-Code)

Visual Studio Code will use c/cpp code extension and tasks to build and clean the workspace. You can use it and follow these steps on Windows or Linux environment.

## Setup

First, you have to install [PVSneslib](https://github.com/alekmaul/pvsneslib/releases/latest) and [Visual Studio Code](https://code.visualstudio.com)
If you are not fluent with the library, please follow instructions on the [Wiki pages](https://github.com/alekmaul/pvsneslib/wiki)

Once it is done, you have to :
* Install the following extensions in Visual Studio Code: `C/C++ for Visual Studio Code`
* Copy the `.vscode` folder to your project directory.
    - In `c_cpp_properties.json` you **must update** `PVSnesLib home directory` with your real installation path.
    For instance, if you are on Windows OS, you should have something like : `"pvsneslibPath": ["c:/snesdev/pvsneslib/include/**","c:/snesdev/pvsneslib/devkitsnes/include"],`
    - If you are on Linux environment, your path should be formated like that `"pvsneslibPath": ["/path/to/pvsneslib/include/**","/path/to/pvsneslib/devkitsnes/include"],`
    - Please do not forget the double "**" as displayed before, it is mandatory to works correctly
    
## Usage

* Open `VSCode` then `File->Open Folder...` and choose the folder of your poject.
* Hit `Ctrl + Shift + B` and on the popup list you will see 2 tasks:
    * `PVSneslib - Clean`: removes all objects and temporary files
    * `PVSneslib - Build`: just compiles the project
* If you do not want to use the shortcuts, you can use the integrated terminal after moving to your project directory