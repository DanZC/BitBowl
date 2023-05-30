#include "input.hpp"

u8 gamepad[4]{};
u8 gamepad_prev[4]{};
u8 gamepad_changed[4]{};

#define UPDATE_GAMEPAD(N) gamepad_prev[N-1] = gamepad[N-1]; \
    gamepad[N-1] = *GAMEPAD##N; \
    gamepad_changed[N-1] = gamepad[N-1] ^ gamepad_prev[N-1];

void input_update()
{
    //Update gamepad state
    UPDATE_GAMEPAD(1);
    UPDATE_GAMEPAD(2);
    UPDATE_GAMEPAD(3);
    UPDATE_GAMEPAD(4);
}

#include "util.hpp"

void input_draw(u8 controller)
{
    char buffer[64];
    //strfmt(buffer, "Input  : %08b\nChanged: %08b", gamepad1, gamepad1_changed);
    strfmt(buffer, "Input  : %#02X\nChanged: %04d", gamepad[controller], gamepad_changed[controller]);
    /*u16 i = 0;
    i = buffer_str(buffer, i, "Input  : ");
    i = buffer_bin(buffer, i, gamepad1, 8);
    buffer[i++] = '\n';
    i = buffer_str(buffer, i, "Changed: ");
    i = buffer_bin(buffer, i, gamepad1_changed, 8);*/
    text(buffer, 0, 48);
}