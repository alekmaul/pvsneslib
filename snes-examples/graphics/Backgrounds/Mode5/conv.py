# SNES Tile Tool
# by thefox <thefox@aspekt.fi>
# Currently only works in Python 2.
# Requires Pillow (PIL).

import argparse
from PIL import Image
import struct

class Error( Exception ):
    def __init__( self, msg ):
        self.msg = msg
    def __str__( self ):
        return str( self.msg )

def warn( msg ):
    print "warning: {}".format( msg )

def unreachable():
    assert False, "unreachable code reached"

def rgb8( image ):
    image = image.convert( "RGB" )
    pixels = image.load()

    result = ""
    rOnes, gOnes, bOnes = 0, 0, 0
    for j in range( image.size[1] ):
        for i in range( image.size[0] ):
            rgbIn = pixels[ i, j ]
            # Format (in tile data) is: bbgggrrr
            # With the extra LSBs (capitalized): bbBgggGrrrR
            rOut = rgbIn[0]//16
            gOut = rgbIn[1]//16
            bOut = rgbIn[2]//32
            # Count the ones in the LSBs to figure out which values to use in
            # the screen data.
            # \todo All pixels with CHR data 0 map to transparent, regardless
            #       of the LSBs in the screen data. Leave those pixels out of
            #       these calculations?
            rOnes += rOut & 1; gOnes += gOut & 1; bOnes += bOut & 1
            result += chr( ( bOut//2 ) << 6 | ( gOut//2 ) << 3 | ( rOut//2 ) )

    numPixels = image.size[0] * image.size[1]
    # If more ones than zeros, use 1. (0 wins ties.)
    lsbR = 1 if rOnes > numPixels//2 else 0
    lsbG = 1 if gOnes > numPixels//2 else 0
    lsbB = 1 if bOnes > numPixels//2 else 0

    # Return the tile and the "palette number" (0..7) for Direct Select.
    return result, lsbB << 2 | lsbG << 1 | lsbR

def mirrorH( rawTile, stride ):
    assert len( rawTile ) % stride == 0
    result = ""
    for i in range( 0, len( rawTile ), stride ):
        result += "".join( reversed( rawTile[ i : i+stride ] ) )
    return result

def mirrorV( rawTile, stride ):
    assert len( rawTile ) % stride == 0
    tmp = [ rawTile[ i : i+stride ] for i in range( 0, len( rawTile ), stride ) ]
    return "".join( reversed( tmp ) )

def toBytes( tile ):
    try:
        return tile.tobytes()
    except AttributeError:
        return tile.tostring()

def process( infile, bpp, tilesize, optimizeDupes, optimizeMirrors, directSelect,
             mode7, screenwise ):
    pilImage = Image.open( infile )

    if mode7:
        if bpp != 8: raise Error( "mode 7 requires bpp 8 (-b 8)" )
        if tilesize != ( 8, 8 ): raise Error( "mode 7 requires tile size 8x8 (-s 8x8)" )
        if optimizeMirrors:
            warn( "mirror tiles can't be optimized in mode 7 (ignoring -Om)" )
            optimizeMirrors = False

    if directSelect:
        if bpp != 8:
            raise Error( "direct select requires bpp 8 (-b 8)" )
    else:
        # Non-paletted images are only allowed for Direct Select.
        if pilImage.mode != "P":
            raise Error( "image must be paletted" )
        if pilImage.palette.mode != "RGB":
            raise Error( "image must have an RGB palette" )

    if pilImage.size[0] % tilesize[0] != 0:
        raise Error( "image width must be a multiple of tile width ({})".format( tilesize[0] ) )
    if pilImage.size[1] % tilesize[1] != 0:
        raise Error( "image height must be a multiple of tile height ({})".format( tilesize[1] ) )

    # Default to considering the whole image a screen.
    screenSize = pilImage.size
    if screenwise:
        screenSize = 32*tilesize[0], 32*tilesize[1]
        if pilImage.size[0] % screenSize[0] != 0:
            raise Error( "image width must be a multiple of screen width ({})".format( screenSize[0] ) )
        if pilImage.size[1] % screenSize[1] != 0:
            raise Error( "image height must be a multiple of screen height ({})".format( screenSize[1] ) )

    mapSizeScreens = map( lambda x: x[0]//x[1], zip( pilImage.size, screenSize ) )
    screenSizeTiles = map( lambda x: x[0]//x[1], zip( screenSize, tilesize ) )

    # AND mask for masking out the palette number from a pixel.
    mask = ( 1 << bpp ) - 1

    # Split the image into tiles. Also generate the tilemap.
    rawTiles = []
    tilemap = []
    optimizedTileIndex = {}
    for sj in range( mapSizeScreens[1] ):
        for si in range( mapSizeScreens[0] ):
            for j in range( screenSizeTiles[1] ):
                for i in range( screenSizeTiles[0] ):
                    x = si*screenSize[0] + i*tilesize[0]
                    y = sj*screenSize[1] + j*tilesize[1]
                    tile = pilImage.crop( ( x, y, x+tilesize[0], y+tilesize[1] ) )

                    if directSelect:
                        # Calculate the 8-bit direct select RGB value from the input.
                        rawTile, rgbLsb = rgb8( tile )
                    else:
                        rawTile = toBytes( tile )

                    assert len( rawTile ) == tilesize[0]*tilesize[1]

                    # Figure out the palette number (0..7) based on the tile's pixels.
                    paletteNum = None
                    if directSelect:
                        paletteNum = rgbLsb
                    else:
                        warned = False
                        for p in rawTile:
                            pixel = ord( p )
                            # If color index 0 is used, the palette doesn't matter, since
                            # it's always transparent.
                            if pixel & mask == 0: continue
                            # \note This is always 0 for bpp==8.
                            pixelPalette = pixel >> bpp
                            # Check for violations.
                            if not warned and paletteNum is not None and paletteNum != pixelPalette:
                                warn( "more than one palette used in the tile at ({}, {})".format( x, y ) )
                                warned = True
                            paletteNum = pixelPalette

                    # Can be None if tile was entirely transparent.
                    if paletteNum is None: paletteNum = 0
                    paletteNum &= 7

                    # Mask out the palette number.
                    rawTile = "".join( map( lambda x: chr( ord( x ) & mask ), rawTile ) )
                    # Generate mirrored versions.
                    rawTileH = mirrorH( rawTile, tilesize[0] )
                    rawTileV = mirrorV( rawTile, tilesize[0] )
                    rawTileHV = mirrorV( rawTileH, tilesize[0] )

                    flipFlags = 0
                    if optimizeDupes:
                        candidates = [ ( rawTile, 0b00 ) ]

                        if optimizeMirrors:
                            candidates.extend( zip(
                                ( rawTileH, rawTileV, rawTileHV ),
                                ( 0b01, 0b10, 0b11 )
                            ) )

                        for tile, flags in candidates:
                            if tile in optimizedTileIndex:
                                flipFlags = flags
                                tileIndex = optimizedTileIndex[ tile ]
                                break
                        else:
                            tileIndex = len( rawTiles )
                            optimizedTileIndex[ rawTile ] = tileIndex
                            rawTiles.append( rawTile )
                    else:
                        tileIndex = len( rawTiles )
                        rawTiles.append( rawTile )

                    # \todo Check if ran over 1024 8x8 tiles (or 256 16x16 tiles,
                    #       or 256 tiles for Mode 7)

                    tilemap.append( ( tileIndex, paletteNum, flipFlags ) )

    # Generate palette.
    snesPalette = None
    if not directSelect:
        snesPalette = []
        palette = pilImage.getpalette()
        paletteLen = len( palette )
        # Restrict length based on current bpp. Also never output more than 256
        # palette entries.
        # \todo Command line option for restricting the size further.
        paletteLen = min( paletteLen, 3 * 8 * 2**bpp, 3*256 )
        for i in range( 0, paletteLen, 3 ):
            snesRgb = reduce(
                lambda x, y: ( x << 5 ) | y,
                reversed( map( lambda x: x//8, palette[ i:i+3 ] ) )
            )
            snesPalette.append( snesRgb )

    # \todo Return tilemap size?
    return rawTiles, tilemap, snesPalette

def packPlane( rowData, plane ):
    assert len( rowData ) == 8

    # Extract the correct bit for the specified bitplane, and pack all of them
    # into a byte.
    bits = map( lambda x: ( ord( x ) >> plane ) & 1, rowData )
    packedByte = reduce( lambda x, y: ( x << 1 ) | y, bits )

    return chr( packedByte )

def packTile( tile, bpp ):
    result = ""

    # For 8x8 tiles, we have 64 chars. Each row is 8 bytes.
    # 16 bytes for 2bpp, 32 bytes for 4bpp, 64 bytes for 8bpp.
    for outputByteIndex in range( 0, 8*bpp ):
        row = outputByteIndex//2 % 8
        plane = 2 * ( outputByteIndex >> 4 ) | ( outputByteIndex & 1 )
        rowData = tile[ 8*row : 8*row+8 ]
        result += packPlane( rowData, plane )

    return result

def crop16x16( tile16x16, rect ):
    assert len( tile16x16 ) == 16*16
    result = ""
    for j in range( rect[1], rect[3] ):
        for i in range( rect[0], rect[2] ):
            result += tile16x16[ 16*j+i ]
    return result

def writeOutput( results, outprefix, bpp, tilesize, mode7 ):
    rawTiles, tilemap, snesPalette = results

    # \todo What file extensions to use?

    if mode7:
        with open( outprefix + ".mo7", "wb" ) as f:
            # Interleave the CHR data and tile indices.
            rawTilesFlat = "".join( rawTiles )
            numEntries = max( len( rawTilesFlat ), len( tilemap ) )
            for i in range( numEntries ):
                data = 0
                if i < len( tilemap ):
                    index = tilemap[ i ][ 0 ]
                    assert 0 <= index <= 255
                    data |= index
                if i < len( rawTilesFlat ):
                    data |= ord( rawTilesFlat[ i ] ) << 8

                f.write( struct.pack( "<H", data ) )
    else:
        with open( outprefix + ".chr", "wb" ) as f:
            if tilesize == ( 8, 8 ):
                for rawTile in rawTiles:
                    f.write( packTile( rawTile, bpp ) )
            elif tilesize == ( 16, 16 ):
                # For 16x16 tiles, split to 8x8 tiles. First output the top half
                # of 8 16x16 tiles, then the bottom half. Repeat for all tiles.
                for k in range( 0, len( rawTiles ), 8 ): # 16x16 tile base index
                    for j in range( 2 ): # Upper (0) or lower (1)
                        for i in range( 8 ): # For each 16x16 tile
                            # If index in range, output the tile as usual. Otherwise
                            # add padding.
                            if k+i < len( rawTiles ):
                                tile16x16 = rawTiles[ k+i ]
                                # 256 bytes in the 16x16 tile, split off 64 bytes (8x8).
                                tile8x8_1 = crop16x16( tile16x16, ( 0, 8*j, 8,   8*j+8 ) )
                                tile8x8_2 = crop16x16( tile16x16, ( 8, 8*j, 8+8, 8*j+8 ) )
                            else:
                                # Padding is not needed on the very last output row.
                                if j == 1 and k//8 == ( len( rawTiles ) - 1 )//8:
                                    continue
                                tile8x8_1 = chr( 0 ) * 64
                                tile8x8_2 = tile8x8_1
                            f.write( packTile( tile8x8_1, bpp ) )
                            f.write( packTile( tile8x8_2, bpp ) )
            else: unreachable()

        with open( outprefix + ".nam", "wb" ) as f:
            for index, paletteNum, flipFlags in tilemap:
                if tilesize == ( 16, 16 ):
                    # SNES wants an 8x8 index, so our 16x16 index needs to be
                    # massaged a little bit.
                    index = index//8*32 + index%8*2
                assert 0 <= index <= 1023
                assert 0 <= paletteNum <= 7
                assert 0 <= flipFlags <= 3
                data = flipFlags << 14 | paletteNum << 10 | index
                # \todo Currently saving as little-endian. Should be an option?
                f.write( struct.pack( "<H", data ) )

    if snesPalette is not None:
        with open( outprefix + ".pal", "wb" ) as f:
            for entry in snesPalette:
                assert 0 <= entry <= 32767
                # \todo Currently saving as little-endian. Should be an option?
                f.write( struct.pack( "<H", entry ) )

def main():
    argParser = argparse.ArgumentParser(
        description="SNES tile conversion tool" )
    argParser.add_argument( "-i", "--infile", required=True,
        help="input image")
    argParser.add_argument( "-b", "--bpp", default=4, type=int,
        choices=[ 2, 4, 8 ], help="bits per pixel" )
    argParser.add_argument( "-s", "--tilesize", default="8x8",
        choices=[ "8x8", "16x16", ], help="tile size" )
    argParser.add_argument( "-w", "--screenwise", action="store_true",
        help="import as 32x32 tile screens")
    argParser.add_argument( "-d", "--directselect", action="store_true",
        help="use CG Direct Select (8 bpp only)")
    argParser.add_argument( "-m7", "--mode7", action="store_true",
        help="generate data in Mode 7 format")
    argParser.add_argument( "-Od", "--optimizedupes", action="store_true",
        help="optimize duplicate tiles")
    argParser.add_argument( "-Om", "--optimizemirrors", action="store_true",
        help="optimize duplicate mirror tiles (needs -Od)")
    argParser.add_argument( "-o", "--outprefix", required=True,
        help="output file prefix" )

    args = argParser.parse_args()

    tilesize = tuple( map( int, args.tilesize.split( "x" ) ) )

    try:
        results = process( args.infile, args.bpp, tilesize,
            args.optimizedupes, args.optimizemirrors, args.directselect,
            args.mode7, args.screenwise )
        writeOutput( results, args.outprefix, args.bpp, tilesize, args.mode7 )
    except Error as e:
        print "error: {}".format( e )

main()