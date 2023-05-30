#pragma once
#include "common.h"

struct Sprite
{
    const u8* image;
    u8 size;
    u8 fSize;
    u8 frame;
    u8 drawColors: 3;
    u8 visible: 1;
    u8 flags: 4;
};

void sprite_draw(const Sprite* sprite, i16 x, i16 y);