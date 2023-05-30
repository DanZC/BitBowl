#pragma once

#include "common.h"
#include "bowling.hpp"
#include "object.hpp"

struct GlobalData;
struct SaveData;

struct GameState
{
    u8 taskId;
    u8 view;
    i16 camPosX;
    i16 camPosY;
    Object objects[16];
    BallBody ballBody;
    PinBody  pinBodies[10];
    u8 playerState;
    u8 ballState;
    u16 pinState;
    u8 playerCount;
    u8 gameKind;
    union {
        struct {
            u8 frameNum;
            u8 throwNum;
        };
        u16 streak;
    };
    u8 turn;
    union {
        u8 viewFrame;
        u8 sparePinCount;
    };
    PlayerData score[4];
    union {
        u16 winningScore;
        u16 finalStreak;
    };
    u8 winningPlayer;
    i8 meterDir;
    u8 cursorFrame;
    u8 cursorOption;
    char nameBuffer[10];
    SaveData* save;
};

#define GAME_KIND_NORMAL    0
#define GAME_KIND_SURVIVAL  1

void game_init(GameState* state, void* data);
void game_update(GameState* state, GlobalData* gdata);
void game_draw(GameState* state);