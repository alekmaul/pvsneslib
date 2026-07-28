#ifndef _MAP32X32_H
#define _MAP32X32_H

// max scroll area (visible area is 16 x 14)
#define MAX_SCROLL_WIDTH_32x32 32*16 - 16*16  // 256
#define MAX_SCROLL_HEIGHT_32x32 32*16 - 14*16 // 288

void setSpriteMap32x32(u16* spritemap, u16 spritemap_len);
u16* getSpriteMap32x32();
u16 getSpriteMap32x32_length();
void initSpriteMap32x32();
void drawSpriteRaw32x32(u8 x, u8 y, u16 sprite);
u16 getSpriteRaw32x32(u8 x, u8 y);
void drawSprite32x32(u8 x, u8 y, u16 sprite);
u16 getSprite32x32(u8 x, u8 y);
u16 element2sprite32x32(u8 elem);
u16 calculateSpriteIndex32x32(u8 elem);
u16 calculateSpritesLength32x32(u16 number_of_sprites);
void screenRefreshPos32x32(u8 x, u8 y, u16 address);
void updateSprite32x32(u8 *source, u16 address, u16 elem);

#endif
