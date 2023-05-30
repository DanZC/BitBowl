#include "sprite.hpp"

void sprite_draw(const Sprite* sprite, i16 x, i16 y)
{
    if(!sprite->visible) return;

    if(x < -sprite->size || x > (SCREEN_SIZE + sprite->size) ||
       y < -sprite->fSize || y > (SCREEN_SIZE + sprite->fSize)) {
        return;
    }

    const u32 fWidth = sprite->size;
    const u32 fHeight = sprite->fSize;

    *DRAW_COLORS = 
        ((sprite->drawColors & 0b100)? 0x4000: 0) |
        ((sprite->drawColors & 0b010)? 0x0300: 0) |
        ((sprite->drawColors & 0b001)? 0x0020: 0) |
        0x0001;


    blitSub(sprite->image, x, y, fWidth, fHeight, 
            0, sprite->fSize * sprite->frame, 
            sprite->size, sprite->flags);
}