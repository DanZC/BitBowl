#include "common.h"
#include "util.hpp"
#include "bowling.hpp"
#include "netplay.hpp"
#include "save.hpp"
#include "global.hpp"
#include "input.hpp"

/*
const u8 frame[] = {
    // Straight piece
    0b00000000,
    0b00000000,
    0b00000000,
    0b11111111,
    0b11111111,
    0b00000000,
    0b00000000,
    0b00000000,
    // Corner piece
    0b00000000,
    0b00000000,
    0b00000000,
    0b00011111,
    0b00011111,
    0b00011000,
    0b00011000,
    0b00011000,
    // T piece
    0b00000000,
    0b00000000,
    0b00000000,
    0b11111111,
    0b11111111,
    0b00011000,
    0b00011000,
    0b00011000,
    // Cross piece
    0b00011000,
    0b00011000,
    0b00011000,
    0b11111111,
    0b11111111,
    0b00011000,
    0b00011000,
    0b00011000,
};

#define frameSTRAGHT 0, 0
#define frameCORNER  0, 8
#define frameT       0, 16
#define frameCROSS   0, 24

#define frameSIZE    8, 8
#define frameWidth   8

u8 taskId;

char get_char_from_throw(u8 t1, u8 t2, u8 ti)
{
    const u8 t = (ti == 0)? t1: t2;
    if(t == 0) return '-';
    if(t == THROW_NULL) return ' ';
    if(ti == 1 && t1 + t2 == 10) return '/';
    if(t == 10) return 'X';
    return (char)('0' + t);
}

void frame_draw(i32 x, i32 y, i32 f, const PlayerData* pd)
{
    *DRAW_COLORS = 0x30;
    blitSub(frame, x+0,  y+0,  frameSIZE, frameCORNER,  frameWidth, BLIT_1BPP);
    blitSub(frame, x+8,  y+0,  frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
    blitSub(frame, x+16, y+0,  frameSIZE, frameT,       frameWidth, BLIT_1BPP);
    blitSub(frame, x+24, y+0,  frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
    blitSub(frame, x+32, y+0,  frameSIZE, frameCORNER,  frameWidth, BLIT_1BPP | BLIT_FLIP_X);

    blitSub(frame, x+0,  y+8,  frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP | BLIT_ROTATE);
    blitSub(frame, x+16, y+8,  frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP | BLIT_ROTATE);
    blitSub(frame, x+32, y+8,  frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP | BLIT_ROTATE);

    blitSub(frame, x+0,  y+16, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP | BLIT_ROTATE);
    //blitSub(frame, x+8,  y+16, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
    blitSub(frame, x+16, y+16, frameSIZE, frameCORNER,  frameWidth, BLIT_1BPP | BLIT_FLIP_Y);
    blitSub(frame, x+24, y+16, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
    blitSub(frame, x+32, y+16, frameSIZE, frameT,       frameWidth, BLIT_1BPP | BLIT_ROTATE | BLIT_FLIP_Y);

    blitSub(frame, x+0,  y+24, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP | BLIT_ROTATE);
    blitSub(frame, x+32, y+24, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP | BLIT_ROTATE);

    blitSub(frame, x+0,  y+32, frameSIZE, frameCORNER,  frameWidth, BLIT_1BPP | BLIT_ROTATE);
    blitSub(frame, x+8,  y+32, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
    blitSub(frame, x+16, y+32, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
    blitSub(frame, x+24, y+32, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
    blitSub(frame, x+32, y+32, frameSIZE, frameCORNER,  frameWidth, BLIT_1BPP | BLIT_ROTATE | BLIT_FLIP_Y);

    *DRAW_COLORS = 0x3;
    char buffer[64];
    auto score = score_player(pd, f + 1);
    const Frame& fr = pd->frames[f];
    if(score >= 0)
    {
        strfmt(buffer, "\n %c %c \n\n %3d \n\n", 
            get_char_from_throw(fr.throws[0], fr.throws[1], 0), 
            get_char_from_throw(fr.throws[0], fr.throws[1], 1), 
            score);
    }
    else 
    {
        strfmt(buffer, "\n %c %c \n\n     \n\n", 
            get_char_from_throw(fr.throws[0], fr.throws[1], 0), 
            get_char_from_throw(fr.throws[0], fr.throws[1], 1));
    }
    text(buffer, x, y);
}

void stat_screen_draw(const GameState* gs)
{
    char buffer[32];
    for(u8 i = 0; i < gs->playerCount; i++)
    {
        strfmt(buffer, "P%c", (char)('1' + (i)));
        text(buffer, 6, 16+(i * 40));
        frame_draw(32-8,  (i * 40),  0, &gs->score[i]);
        frame_draw(64-8,  (i * 40),  1, &gs->score[i]);
        frame_draw(96-8,  (i * 40),  2, &gs->score[i]);
        frame_draw(128-8, (i * 40),  3, &gs->score[i]);
    }
}*/

GlobalData gData;

void start()
{
    PALETTE[0] = PALETTE_COLOR_0;
    PALETTE[1] = PALETTE_COLOR_1;
    PALETTE[2] = PALETTE_COLOR_2;
    PALETTE[3] = PALETTE_COLOR_3;

    // Load save data if any
    save_read(&gData.save);

    if(!(gData.save.header[0] == 'S'
    && gData.save.header[1] == 'A'
    && gData.save.header[2] == 'V'
    && gData.save.header[3] == '\0'))
    {
        // Save is empty or corrupted.
        // Reset save data to initial state.
        save_init(&gData.save);
    }

    // Set initial state
#if SOUND_TEST
    state_set(&gData, STATE_SOUNDTEST, nullptr);
#else
    state_set(&gData, STATE_TITLE, nullptr);
#endif
}

void update () 
{
    input_update();
    state_update(&gData);
    state_draw(&gData);

#if 0
    *DRAW_COLORS = 0x41;
    char buffer[64];
    strfmt(buffer, "%c%d", (NETPLAY_ACTIVE)? 'N': '.', 
        NETPLAY_PIDX);
    text(buffer, 0, 144);
#endif 
}
