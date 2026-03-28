#ifndef _GFX4SNES_GRADIENTS_H
#define _GFX4SNES_GRADIENTS_H

#include "errors.h"

#include "images.h"

//-------------------------------------------------------------------------------------------------
extern void gradient_extractcolors(t_image *img, bool isquiet);
extern void gradient_save (const char *filename, bool isquiet);

#endif
