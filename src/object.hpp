#pragma once
#include "common.h"
#include "sprite.hpp"

struct Object
{
    i16 x;
    i16 y;
    Sprite sprite;
};

void object_init(Object* obj, i16 x, i16 y, u8 fSize, u8 drawColors, const u8* image, u8 imageSize, u8 imageFlags);
void object_draw_ordered(const Object* obj, i16 vx, i16 vy);
void object_set_pos(Object* obj, i16 x, i16 y);