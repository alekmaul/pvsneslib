# SNS Sample: ObjectSize

## Objective of the Example

The purpose of the sample is to display and dynamically Sprites of different sizes. 

The sample demonstrates:

- how to load sprite data with oamInitGfxSet and direct DMA
- dhow to ynamically load data in VRAM (with the brightness management)

## Change Log

2023-02-26: By Dienben

- Add a parameter for Background 1 address
- Update of the ADRPRITE to avoid collision with the background address and so change the sprite offset
- Update the ADRSPRITLARDGE to avoid collision with the ADRPRITE address and so change the sprite offset
- Change Sprite priority (no effect but could be usefull for other sample)

Tested on Mac M1 with bsnes+
