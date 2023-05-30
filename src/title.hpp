#pragma once

#include "common.h"

struct GlobalData;
struct SaveData;

struct TitleState
{
    u8 option;
    u8 selTimer: 7;
    u8 fastSelection: 1;
    u8 taskId;
    u8 view;
    u8 option2;
    u8 optionFlags;
    SaveData* save;
};

void title_init(TitleState* state, void* data);
void title_update(TitleState* state, GlobalData* gdata);
void title_draw(TitleState* state);