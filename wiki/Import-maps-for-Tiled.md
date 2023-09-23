This tutorial will help you to understand how to use the PVSnesLib map and object engines. The source code is available in examples part of pvsneslib.

We are going to do the first level of Commander Keen 1, which was a great game on PC (https://en.wikipedia.org/wiki/Commander_Keen_in_Invasion_of_the_Vorticons).

The map used in the tutorial is this one:
<a target="_blank" href="https://user-images.githubusercontent.com/2528347/198873690-096676c1-71af-4082-93ca-a2cf0a7a04c1.png"><img src="https://user-images.githubusercontent.com/2528347/198873690-096676c1-71af-4082-93ca-a2cf0a7a04c1.png"></a>

Click on the map and save it on your hard drive, inside a directory for this tutorial.

## Install Tiled

Go to https://www.mapeditor.org/ and download Tiled for your operating system (download is available at https://thorbjorn.itch.io/tiled).  

Warning ! You must use a V1.9.x to be compatible with tmx2snes tool.

## Create a tmx compatible file from a bitmap

Go to https://portabledev.com/pvsneslib/tilesetextractor/ and upload the png file your saved from Commander Keen 1 game with the "Choose File" button on the top left of the screen.

The tool will create tileset and tmx files, as below.

<img width="300" src="https://user-images.githubusercontent.com/2528347/198873692-5e1ee6d1-5ca0-4c11-a40c-196acb4853b4.png">

Save the 2 files shown below in same directory where you saved the PNG map file of the game.

<img width="300" src="https://user-images.githubusercontent.com/2528347/198873694-4672ac17-2b43-4978-8770-ad2c188ac272.png">

Use your graphic editor software to change color depth of tiles to 256 colors without alpha channel (or gfx4snes will not work...). 
In our example, the number of colors will certainly be OK, we're just validating that the extractor set the number of colors correctly.



<img width="300" src="https://user-images.githubusercontent.com/2528347/198880302-c959856b-8f34-4a8c-be0e-e6e062249249.png">

I used GraphicGale for that (https://graphicsgale.com/us/).

Now, double-click on tmx file, it will open Tiled with your png file converted to a map file compatible with PVSnesLib!

<img width="400" src="https://user-images.githubusercontent.com/2528347/198880481-b873e585-757f-4f8b-99e9-ed06cfab5bd7.png">

Next tutorial will explain how to update map in Tiled for collision detection and how to add objects.
