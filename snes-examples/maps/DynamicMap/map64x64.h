#ifndef _MAP64X64_H
#define _MAP64X64_H

#define MAX_SCROLL_WIDTH_64x64 64*16 - 16*16  // 768
#define MAX_SCROLL_HEIGHT_64x64 64*16 - 14*16 // 800

void setSpriteMap64x64(u16* spritemap, u16 spritemap_len);
u16* getSpriteMap64x64(u16* spritemap);
u16 getSpriteMap64x64_length();
void initSpriteMap64x64();
void drawSpriteRaw64x64(u8 x, u8 y, u16 sprite);
u16 getSpriteRaw64x64(u8 x, u8 y);
void drawSprite64x64(u8 x, u8 y, u16 sprite);
u16 getSprite64x64(u8 x, u8 y);
u16 element2sprite64x64(u8 elem);
u16 calculateSpriteIndex64x64(u8 elem);
u16 calculateSpritesLength64x64(u16 number_of_sprites);
void screenRefreshPos64x64(u8 x, u8 y, u16 address);
void updateSprite64x64(u8 *source, u16 address, u16 sprite);

#endif
