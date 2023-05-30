#include "sound_test.hpp"
#include "sound.hpp"
#include "task.hpp"
#include "input.hpp"
#include "util.hpp"

enum SoundOptions
{
    SND_MOVE_SEL,
    SND_THROW_BALL,
    SND_HIT_PIN,
    SND_BACKSPACE,
    SND_ENTER_CHAR,
    ME_GUTTER,
    ME_SPARE,
    ME_STRIKE,
    SND_OPT_COUNT
};

void sound_test_init(SoundTestState* state, UNUSED void* data)
{
    state->option = 0;
    state->fastSelection = 0;
    state->selTimer = 0;
    state->taskId = TASKID_INVALID;
    state->view = 0;
}

void sound_test_update(SoundTestState* state, UNUSED GlobalData* gdata)
{
    if(state->taskId != TASKID_INVALID)
    {
        if(!task_update(state->taskId, gdata)) return;
        task_destroy(state->taskId);
        state->taskId = TASKID_INVALID;
        return;
    }

    if(GAMEPAD_JUST_PRESSED(0, BUTTON_1))
    {
        switch (state->option)
        {

        case SND_MOVE_SEL:
            sound_move_selection();
            state->taskId = task_wait_frames(4);
            break;

        case SND_THROW_BALL:
            sound_throw_ball();
            state->taskId = task_wait_frames(1);
            break;

        case SND_HIT_PIN:
            sound_hit_pin(0);
            state->taskId = task_wait_frames(2);
            break;

        case SND_BACKSPACE:
            sound_backspace();
            state->taskId = task_wait_frames(2);
            break;
        
        case SND_ENTER_CHAR:
            sound_enter_char();
            state->taskId = task_wait_frames(4);
            break;

        case ME_GUTTER:
            state->taskId = sound_me_gutter();
            break;

        case ME_SPARE:
            state->taskId = sound_me_spare();
            break;

        case ME_STRIKE:
            state->taskId = sound_me_strike();
            break;
        
        default:
            break;
        }
        return;
    }

    if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_LEFT) || GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_RIGHT))
    {
        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_LEFT)) 
        {
            if(state->option == 0) state->option = SND_OPT_COUNT - 1;
            else state->option--;
            state->fastSelection = 0;
            state->selTimer = 20;
        } 
        else if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_RIGHT)) 
        {
            if(state->option == SND_OPT_COUNT - 1) state->option = 0;
            else state->option++;
            state->fastSelection = 0;
            state->selTimer = 20;
        }
        sound_move_selection();
    }

    if(state->selTimer > 0)
    {
        state->selTimer--;
    }
    else 
    {
        if(GAMEPAD_PRESSED(PAD(1), BUTTON_LEFT)) 
        {
            if(state->option == 0) state->option = SND_OPT_COUNT - 1;
            else state->option--;
            state->fastSelection = 1;
            state->selTimer = 10;
            sound_move_selection();
            //tone(260, 4, 40, TONE_PULSE1 | TONE_MODE3);
        } 
        else if(GAMEPAD_PRESSED(PAD(1), BUTTON_RIGHT)) 
        {
            if(state->option == SND_OPT_COUNT - 1) state->option = 0;
            else state->option++;
            state->fastSelection = 1;
            state->selTimer = 10;
            sound_move_selection();
        }
        else 
        {
            state->fastSelection = 0;
            state->selTimer = 20;
        }
    }
}

void sound_test_draw(SoundTestState* state)
{
    char buffer[64];
    text("Sound Test", 0, 0);

    strfmt(buffer, "Track %d of %d", state->option + 1, SND_OPT_COUNT);

    text(buffer, 0, 16);

    if(state->taskId != TASKID_INVALID)
    {
        text("Waiting...", 0, 24);
    }
}