#ifndef _TMX2SNES_TMXCONV_H
#define _TMX2SNES_TMXCONV_H

#include <stdbool.h>

// Reads a native Tiled .tmx map file (plus any external .tsx tileset it
// references) and returns a NUL-terminated buffer containing the equivalent
// Tiled-JSON (.tmj) text -- i.e. exactly what "File > Export As... > JSON"
// would have produced. The returned buffer is meant to be fed directly into
// cute_tiled_load_map_from_memory(), so no other code needs to change.
//
// Caller owns the returned buffer and must free() it.
char *tmx_convert_to_json(const char *tmxpath, bool isquiet);

#endif
