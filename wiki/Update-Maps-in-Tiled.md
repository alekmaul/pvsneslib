You must now have a tmx file with a tileset properly configured for Tiled. 
We need now to update tile properties for map engine.
If not, read 1st part of the tutorial to create it (https://github.com/alekmaul/pvsneslib/wiki/Import-maps-for-Tiled)

## Update tile properties

On the screen below, click on "Edit Tileset" button to open a new tab with tileset properties.

<img width="300" src="https://user-images.githubusercontent.com/2528347/199168309-7d0eabf0-f314-48e4-b199-b2d89a7c927c.png">

If you used the converter tool described in Part1, you will have 3 properties for each tile ("attribute", "palette" and "priority"). If not, select all the tiles on the right with the mouse and use the "+" button on the bottom left to add the 3 properties.

<img width="400" src="https://user-images.githubusercontent.com/2528347/199168311-0d65f788-1fdb-42a9-b549-74ed8feee85b.png">

Now select the tiles as shown below (red rectangles to show the tiles) to change their "attribute" property to FF00 to describe them as blocker. Our hero will not be able to pass through them.

<img width="500" src="https://user-images.githubusercontent.com/2528347/199170180-98bdf0f9-992a-44cf-aa08-a8c2a0be923e.png">

Do the same with pillars (again, red rectangles to show the tiles) to change priority property to 1, to allow our hero to pass behind them.

<img width="500" src="https://user-images.githubusercontent.com/2528347/199170185-6f9115a9-4e0d-4580-a16a-06947b0abf03.png">

If you have a tileset with multiple palettes, you can do the same with the "palette" property of each tile.

The property "attribute" is special, some values are managed by the map/object engines.
* **FF00** is for solid tiles, objects can't pass through them
* **0002** will change action property of object to ACT_BURN value (see <a href="https://github.com/alekmaul/pvsneslib/blob/master/pvsneslib/include/snes/object.h">object.h</a> file of PVSneslib)
* **0004**  will change action property of object to ACT_DIE value (see <a href="https://github.com/alekmaul/pvsneslib/blob/master/pvsneslib/include/snes/object.h">object.h</a> file of PVSneslib)

_"attribute" property value **0002** or **0004** will need to be managed in your code._

## Export map in jSON format

To export your map in a format usable with PVSnesLib, you need to click on the first tab named **tiled.tmx**, to be on the map file and not on the tiles part of the map.

<img width="300" src="https://user-images.githubusercontent.com/2528347/199171995-261bc3d5-94c2-4404-999a-d3d889b4df09.png">

Then, choose **File/Export** and save the file in json format. Name the file **tiled.tmj** (Type is **JSON map files(*.tmj *.json)**). Next time, Tiled will not ask you about a filename as you previously saved your file in json format.

<img width="300" src="https://user-images.githubusercontent.com/2528347/199171998-588929f3-19c8-4027-a36e-c6725e0ab25e.png">

Next tutorial will explain how to code the first part to display the map on screen.
