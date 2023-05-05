

As described on [wikipedia](https://en.wikipedia.org/wiki/Visual_Studio_Code), _Visual Studio Code is a source-code editor made by Microsoft for Windows, Linux and macOS. Features include support for debugging, syntax highlighting, intelligent code completion, snippets, code refactoring, and embedded Git._

![Visual Studio Code](https://upload.wikimedia.org/wikipedia/commons/e/e9/VS_Code_%28Insiders%29.png)
 
It is the tool we recommend to develop with PVSneslib on any operating system and we will show you how to configure it.

### Download

First, please [download](https://code.visualstudio.com/download) the version corresponding to your system then install it and validate that the tool works correctly.

### Configure

When you will open some type of files, Visual studio will suggest you to install extensions. You have to install **C/C++ for Visual Studio Code**

From now, you can already use it to develop, the native functionalities are probably enough but it is a big advantage if you have also code completion!

_If you did not installed PVSneslib, please follow the instructions in [wiki pages](https://github.com/alekmaul/pvsneslib/wiki) before continuing._

Once the framework is installed (from latest [release](https://github.com/alekmaul/pvsneslib/releases/latest) version or from [repository](https://github.com/alekmaul/pvsneslib/wiki/Compiling-from-sources)), go in the root folder of your installation then open **.vscode** folder which contains 2 files.
Open c_cpp_properties.json with your favorite text editor and update the **pvsneslibPath** variable to match your include directories in the installation folder.

For example, if you installed it in `C:/pvsneslib`, you should have
`"pvsneslibPath": ["c:/pvsneslib/pvsneslib/include/**","c:/pvsneslib/devkitsnes/include"],`


### Use it

To use it, you can duplicate the **.vscode** folder template in each folder of your project or just open the whole PVSneslib root folder.

You have an integrated terminal (which require one new configuration step on Windows) in the software (by clicking on "_Terminal_ > _New Terminal_" from the menu) and depending of your preferences, you can :

- use the terminal to navigate in the folder you work with then run `make` commands (you can also right click on the folder you want to build in the browser view then "_open in the integrated terminal_" to avoid navigating with `cd` commands in the terminal. If you are on Windows, it will require one more configuration step. Please read it after !)

- Use `Ctrl + Shift + B` shortcut to access to the 2 tasks to build or clean the project.

### Integrated terminal on Windows

On Windows, the library requires to use MSYS2 with the UCRT64 binary.

If you use the integrated terminal in vscode, you will have to update the user settings by adding this block :

`"terminal.integrated.profiles.windows": {
    "MSYS2 UCRT": {
        "path": "cmd.exe",
        "args": [
            "/c",
            "C:\\msys64\\msys2_shell.cmd -defterm -here -no-start -ucrt64"
        ]
    }
},`

and the workspace settings with this one :

`"terminal.integrated.defaultProfile.windows": "MSYS2 UCRT"`
