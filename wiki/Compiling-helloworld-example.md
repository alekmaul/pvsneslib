If you have installed successfully PVSneslib, you also have snes-examples installed.

Go to snes-examples/hello_world to have the famous Hello World example.

# Tips to help you with coding

In this example, all the source code are in the **src** directory. It consists of only one file **hello_world.c**.

You can change this behavior by adding a SRC variable in the makefile.

It works if you set the SRC variable before the include of snes_rules :
```bash
SRC=./source

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules)`
```

If you are skilled with assembly code, you can also add another variable to allow the creation of **.dbg** files, each debug file is create during compilation process.

```bash
export PVSNESLIB_DEBUG=1

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules)`
```

# Explanation of main code

As any C program, the main function is mandatory and is the entry point automatically called on boot.

Here is the complete code of **Hello World** example:

```bash
#include <snes.h>

extern char tilfont, palfont;

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize text console with our font
    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &tilfont, &palfont);

    // Init background
    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Draw a wonderful text :P
    consoleDrawText(10, 10, "Hello World !");
    consoleDrawText(6, 14, "WELCOME TO PVSNESLIB");
    consoleDrawText(3, 18, "HTTPS://WWW.PORTABLEDEV.COM");

    // Wait for nothing :P
    setScreenOn();

    while (1)
    {
        WaitForVBlank();
    }
    return 0;
}
```

The SNES initialization is now handled automatically by the runtime system, so you can start directly with your program logic. If you need console text functionality, you can initialize the text console with:

```bash
    // Initialize text console with our font
    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &tilfont, &palfont);
```

Then, as we want to use the default font engine, we need to prepare text VRAM entries with:

```bash
    // Initialize text console with our font
    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &tilfont, &palfont);
```

After that, we must declare where the SNES need to put the background in VRAM and we put the SNES in mode 1, which is the most popular mode for games:

```bash
    // Init background
    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
```

That's all, just after, we put the text and wait for an inifite loop!
    
```bash
    // Draw a wonderful text :P
    consoleDrawText(10, 10, "Hello World !");
    consoleDrawText(6, 14, "WELCOME TO PVSNESLIB");
    consoleDrawText(3, 18, "HTTPS://WWW.PORTABLEDEV.COM");

    // Wait for nothing :P
    setScreenOn();

    while (1)
    {
        WaitForVBlank();
    }
    return 0;
```

# How the graphic of the font is used in the code

The font used for **Hello World** example is in main directory. You can change it but colors must stay with only the 1st 16 colors palette.

![image](https://github.com/alekmaul/pvsneslib/assets/2528347/4110969c-5ab2-4181-9446-8d8fcc27e5e4)

The font is declare outside the **hello_world.c** file, but we need to add an external declaration to allow **consoleInitText**  to work:

```bash
extern char tilfont, palfont;
```

Then, in **data.asm** file, you declare all the assets compiled with PVSneslib tools. In our case, only the font:

```bash
.section ".rodata1" superfree

tilfont:
.incbin "pvsneslibfont.pic"

palfont:
.incbin "pvsneslibfont.pal"

.ends
```
> You must remember that each section must be lower than 32 Kilobytes. If you want more space, just add other sections with another name than .rodata1 (like .fontdata for example)

# How to compile

In a command line window, in **hello_world** directory, just type:

```bash
make clean
```

and then

```bash
make 
```
It will produce a large amount of lines to explain what's going on (it's also here where you can see errors).

![image](https://github.com/alekmaul/pvsneslib/assets/2528347/78dadffa-bc9f-4391-b99c-066325c5c8ce)

you will have a **.sfc** file produced name **hello_world.sfc**.

When you run it in no$sns, you will see this screen:

![image](https://github.com/alekmaul/pvsneslib/assets/2528347/3a8d8d9c-8e23-4f19-a44f-db1221f63f01)

That's all! you're ready to do a great SNES game for our awesome console. Try to look at all examples to understand how to use joypad, other graphic modes, and so on.
