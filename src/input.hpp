#ifndef INPUT_HPP
#define INPUT_HPP

#include "common.h"

extern u8 gamepad[4];
extern u8 gamepad_prev[4];
extern u8 gamepad_changed[4];

#define PAD(N) (N-1)
#define GAMEPAD_JUST_PRESSED(N,X) ((gamepad[N] & X) && (gamepad_changed[N] & X))
#define GAMEPAD_JUST_RELEASED(N,X) (!(gamepad[N] & X) && (gamepad_changed[N] & X))
#define GAMEPAD_PRESSED(N,X) ((gamepad[N] & X))
#define GAMEPAD_RELEASED(N,X) (!(gamepad[N] & X))

void input_update();
void input_draw(u8 controller);

#endif //INPUT_HPP