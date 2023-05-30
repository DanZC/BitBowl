#include "object.hpp"
#include "util.hpp"

void object_init(Object* obj, i16 x, i16 y, u8 fSize, u8 drawColors, const u8* image, u8 imageSize, u8 imageFlags)
{
    obj->x = x;
    obj->y = y;
    obj->sprite.frame = 0;
    obj->sprite.fSize = fSize;
    obj->sprite.drawColors = drawColors;
    obj->sprite.visible = 1;
    obj->sprite.image = image;
    obj->sprite.size = imageSize;
    obj->sprite.flags = imageFlags;
}

void object_draw_ordered(const Object* obj, i16 vx, i16 vy)
{
    Object arr[16];
    memcpy((u8*)arr, (const u8*)obj, sizeof(Object) * 16);

    bool swapped = true;
    while(swapped)
    {
        swapped = false;
        for(int i = 0; i < 15; i++)
        {
            Object* curr = &arr[i];
            Object* next = &arr[i+1];

            if(curr->y + (curr->sprite.fSize / 2) > next->y + (next->sprite.fSize / 2))
            {
                memswap((u8*)curr, (u8*)next, sizeof(Object));
                swapped = true;
            }
        }
    }

    for(int i = 0; i < 16; i++)
    {
        sprite_draw(&arr[i].sprite, arr[i].x - vx, arr[i].y - vy);
    }
}

void object_set_pos(Object* obj, i16 x, i16 y)
{
    obj->x = x;
    obj->y = y;
}