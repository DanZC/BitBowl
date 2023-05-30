#pragma once
#include "common.h"

// Empty throw value. Zero is zero pins hit.
constexpr u8 THROW_NULL   = 255;

// Special version of zero pins hit (gutter ball).
constexpr u8 THROW_GUTTER = 128;

struct Frame
{
    u8 index;
    u8 throws[3];
};

struct PlayerData
{
    Frame frames[10];
};

struct PinData
{
    u16 pins: 10;
    u16 flags: 6;
};

struct BallData
{
    // Determines how much the ball sways and which direction.
    // Negative values sway to the left. Positive values sway to
    // the right.
    i16 sway;

    // Determines how fast the ball travels and how much the pins
    // move when contacting the ball.
    u16 power;

    // How much the ball moves on its initial path before swaying.
    u16 skid;
};

// A physics body which determines the movement of the ball.
struct BallBody
{
    BallData data;
    u8 state;
    u16 frameCount;
    f32 dx;
    f32 dy;
    f32 x;
    f32 y;
};

enum {
    PIN_STANDING,
    PIN_ROCKING,
    PIN_FALLING,
    PIN_DOWN,
    PIN_STATE_CHANGED = 8
};

struct PinBody
{
    u8 pinState;
    u16 frameCount;
    //u16 power;
    f32 dx;
    f32 dy;
    f32 x;
    f32 y;
};

void do_ball_physics(BallBody* body, u32 frameCount);
void handle_ball_collisions(BallBody* body, PinBody* pinBodies, u8 pinBodyCount, u8 optionFlags);
void handle_pin_collisions(PinBody* pinBodies, u8 pinBodyCount);
u16 check_pins(PinBody* pinBodies);

constexpr bool is_split(const u16 pins)
{
    if(pins == 0b1111111111) return false;
    if(pins == 0b0000000000) return false;
    int pinDistCount = 0;
    for(int i = 0; i < 10; i++)
    {
        if((pins >> i) & 1) {
            if(pinDistCount >= 4)
                return true;
            else 
                pinDistCount = 0;
        }
        else 
        {
            pinDistCount++;
        }
    }
    return false;
}

static_assert(!is_split(0b1111'111'111));
static_assert(!is_split(0b0001'111'111));
static_assert(!is_split(0b1110'011'111));
static_assert( is_split(0b1110'000'001));
static_assert( is_split(0b0010'000'001));

i32 score_player(const PlayerData* pd, i32 frame);

void init_player_data(PlayerData* data);


