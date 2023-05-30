#pragma once
#include "common.h"
#include "game.hpp"
#include "title.hpp"
#include "save.hpp"
#if SOUND_TEST
#include "sound_test.hpp"
#endif

enum StateType {
    STATE_GAME,
    STATE_TITLE,
#if SOUND_TEST
    STATE_SOUNDTEST,
#endif
};

struct GlobalData
{
    SaveData save;
    StateType type;
    union {
        GameState game;
        TitleState title;
#if SOUND_TEST
        SoundTestState soundTest;
#endif
    };
};

void state_set   (GlobalData* gdata, StateType type, void* data);
void state_init  (GlobalData* gdata, void* data);
void state_update(GlobalData* gdata);
void state_draw  (GlobalData* gdata);