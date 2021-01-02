# Setting up PVSneslib with Visual Studio Code

This small project will help to setup a working environment for Super Nintendo development with PVSneslib toolchain.  

Visual Studio Code will use c/cpp code extension and tasks to build and clean the workspace.  

This project has been tested with Windows environment.


## Prerequisites

. PVSneslib: https://github.com/alekmaul/pvsneslib/releases
.Visual Studio Code: https://code.visualstudio.com


## Setup

* Extract PVSneslib to a directory  (e.g. "C:\snesdev") like explained in wiki (https://github.com/alekmaul/pvsneslib/wiki/Installation-with-Windows).
* Create an environment variable `PVSNESLIB_HOME` pointing to the PVSneslib directory in unix like format (e.g. "/c/snesdev"). 
* Install Visual Studio Code.
* Install the following extensions in vsCode: `C/C++ for Visual Studio Code`
* Copy the `.vscode` folder to you project directory.
    - In `c_cpp_properties.json` you **must update** `PVSnesLib home directory` with Windows like folder name: `"pvsneslibPath": ["c:/snesdev/pvsneslib/include/**","c:/snesdev/pvsneslib/devkitsnes/include"],`
    
=== Usage
* Open `VSCode` and `File->Open Folder...` and choose the folder of your poject.
* Hit `Ctrl + Shift + B` and on the popup list you will see 2 tasks:
    * `PVSneslib - Clean`: removes all objects files
    * `PVSneslib - Build`: just compiles the project
