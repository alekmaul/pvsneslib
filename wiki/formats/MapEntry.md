# SNES Map entry

This format is used by the bgInitMapSet function;

A map entry is a 32x32 array of unsigned short (16 bits) in SNES format :
    
    High     | Low
    -------- | --------
    vhopppcc | cccccccc
    
    Legend:
     - c: Starting character (tile) number
     - h: horizontal flip
     - v: vertical flip
     - p: palette number
     - o: priority bit

