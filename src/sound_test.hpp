#pragma once

#include "common.h"

struct GlobalData;
struct SaveData;

struct SoundTestState
{
    u8 option;
    u8 selTimer: 7;
    u8 fastSelection: 1;
    u8 taskId;
    u8 view;
};

void sound_test_init(SoundTestState* state, void* data);
void sound_test_update(SoundTestState* state, GlobalData* gdata);
void sound_test_draw(SoundTestState* state);