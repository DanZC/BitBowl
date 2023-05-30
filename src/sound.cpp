#include "sound.hpp"
#include "task.hpp"
#include "util.hpp"
#include "pitch_constants.hpp"

#define NOTE_OFF(X) (X*9)
#define BEAT(N)     (N*9)
#define QUARTER_NOTE 9
#define HALF_NOTE    18
#define WHOLE_NOTE   27

struct SoundTask
{
    u16 timer;
};

void sound_move_selection(void)
{
    tone(260, 4, 40, TONE_PULSE1 | TONE_MODE3);
}

void sound_backspace(void)
{
    tone(390 | (260 << 16), 2, 40, TONE_PULSE1 | TONE_MODE1);
}

void sound_enter_char(void)
{
    tone(650 | (550 << 16), 4, 40, TONE_PULSE1 | TONE_MODE1);
}

void sound_throw_ball(void)
{
    tone(100, 1, 80, TONE_PULSE1 | TONE_MODE1);
}

void sound_hit_pin(u32 flags)
{
    tone((240 << 16) | 150, (1 << 8) | 1, 80, TONE_NOISE | flags);
}

void sound_me_init(Task* task, u8* data)
{
    memcpy(task->data, data, sizeof(SoundTask));
}

u8 sound_me_gutter(void)
{
    SoundTask stask{0};
    return task_create(sound_me_init,
    [](Task* task, UNUSED GlobalData* gdata) -> bool {
        auto stask = (SoundTask*)task->data;
        switch(stask->timer)
        {
        case NOTE_OFF(0):
            tone(PITCH_A3, BEAT(1), 60, TONE_PULSE1);
            tone(PITCH_C4, BEAT(1), 60, TONE_PULSE2);
            tone(PITCH_A2, BEAT(1), 40, TONE_TRIANGLE);
            stask->timer++;
            break;
        case NOTE_OFF(1):
            tone(PITCH_A3, BEAT(1), 60, TONE_PULSE1);
            tone(PITCH_C4, BEAT(1), 60, TONE_PULSE2);
            tone(PITCH_G2, BEAT(1), 40, TONE_TRIANGLE);
            stask->timer++;
            break;
        case NOTE_OFF(2):
            tone(PITCH_A3, BEAT(1), 60, TONE_PULSE1);
            tone(PITCH_C4, BEAT(1), 60, TONE_PULSE2);
            tone(PITCH_A2, BEAT(1), 40, TONE_TRIANGLE);
            stask->timer++;
            break;
        case NOTE_OFF(3):
            tone(PITCH_Gs3, BEAT(3), 60, TONE_PULSE1);
            tone(PITCH_B3,  BEAT(3), 60, TONE_PULSE2);
            tone(PITCH_Gs2, BEAT(2), 40, TONE_TRIANGLE);
            stask->timer++;
            break;
        case NOTE_OFF(5):
            tone(PITCH_D2,  BEAT(1), 40, TONE_TRIANGLE);
            stask->timer++;
            break;
        case NOTE_OFF(11):
            return true;
        default:
            stask->timer++;
            return false;
        }
        return false;
    },
    (u8*)&stask);
}

u8 sound_me_spare(void)
{
    SoundTask stask{0};
    return task_create(sound_me_init,
    [](Task* task, UNUSED GlobalData* gdata) -> bool {
        auto stask = (SoundTask*)task->data;
        switch(stask->timer)
        {
        case NOTE_OFF(0):
            tone(PITCH_C4,  9, 60, TONE_PULSE1);
            tone(PITCH_G4,  9, 60, TONE_PULSE2);
            tone(PITCH_G2,  9, 50, TONE_TRIANGLE);
            stask->timer++;
            break;
        case 9:
            tone(PITCH_E4,  9, 60, TONE_PULSE1);
            tone(PITCH_B4,  9, 60, TONE_PULSE2);
            //tone(PITCH_A3,  9, 60, TONE_PULSE1);
            //tone(PITCH_C4,  9, 60, TONE_PULSE2);
            //tone(PITCH_G2,  9, 40, TONE_TRIANGLE);
            tone(PITCH_C2,  9, 50, TONE_TRIANGLE);
            stask->timer++;
            break;
        case 18:
            tone(PITCH_G4,  9, 60, TONE_PULSE1);
            tone(PITCH_C5,  9, 60, TONE_PULSE2);
            //tone(PITCH_A3,  9, 60, TONE_PULSE1);
            //tone(PITCH_C4,  9, 60, TONE_PULSE2);
            //tone(PITCH_A2,  9, 40, TONE_TRIANGLE);
            tone(PITCH_C3,  9, 50, TONE_TRIANGLE);
            stask->timer++;
            break;
        case 27:
            tone(PITCH_G4,  27, 60, TONE_PULSE1);
            tone(PITCH_C5,  27, 60, TONE_PULSE2);
            //tone(PITCH_Gs3, 27, 60, TONE_PULSE1);
            //tone(PITCH_B3,  27, 60, TONE_PULSE2);
            tone(PITCH_G3,  18, 50, TONE_TRIANGLE);
            stask->timer++;
            break;
        case 45:
            tone(PITCH_C3,   9, 40, TONE_TRIANGLE);
            stask->timer++;
            break;
        case 100:
            return true;
        default:
            stask->timer++;
            return false;
        }
        return false;
    },
    (u8*)&stask);
}

u8 sound_me_strike(void)
{
    SoundTask stask{0};
    return task_create(sound_me_init,
    [](Task* task, UNUSED GlobalData* gdata) -> bool {
        auto stask = (SoundTask*)task->data;
        switch(stask->timer)
        {
        case NOTE_OFF(0):
            tone(PITCH_E4, BEAT(2), 60, TONE_PULSE1);
            tone(PITCH_C4, BEAT(2), 60, TONE_PULSE2);
            tone(PITCH_G2, BEAT(2), 50, TONE_TRIANGLE);
            stask->timer++;
            break;
        case NOTE_OFF(2):
            tone(PITCH_C4, BEAT(1), 60, TONE_PULSE1);
            tone(PITCH_G3, BEAT(1), 60, TONE_PULSE2);
            //tone(PITCH_A3,  9, 60, TONE_PULSE1);
            //tone(PITCH_C4,  9, 60, TONE_PULSE2);
            //tone(PITCH_G2,  9, 40, TONE_TRIANGLE);
            tone(PITCH_C2, BEAT(1), 50, TONE_TRIANGLE);
            stask->timer++;
            break;
        case NOTE_OFF(3):
            tone(PITCH_C4, BEAT(1), 60, TONE_PULSE1);
            tone(PITCH_G3, BEAT(1), 60, TONE_PULSE2);
            //tone(PITCH_A3,  9, 60, TONE_PULSE1);
            //tone(PITCH_C4,  9, 60, TONE_PULSE2);
            //tone(PITCH_G2,  9, 40, TONE_TRIANGLE);
            tone(PITCH_C2, BEAT(1), 50, TONE_TRIANGLE);
            stask->timer++;
            break;
        case NOTE_OFF(4):
            tone(PITCH_E4, BEAT(2), 60, TONE_PULSE1);
            tone(PITCH_C4, BEAT(2), 60, TONE_PULSE2);
            //tone(PITCH_G4,  9, 60, TONE_PULSE1);
            //tone(PITCH_C5,  9, 60, TONE_PULSE2);
            //tone(PITCH_A3,  9, 60, TONE_PULSE1);
            //tone(PITCH_C4,  9, 60, TONE_PULSE2);
            //tone(PITCH_A2,  9, 40, TONE_TRIANGLE);
            tone(PITCH_C3, BEAT(1), 50, TONE_TRIANGLE);
            stask->timer++;
            break;
        case NOTE_OFF(6):
            tone(PITCH_G4, BEAT(2), 60, TONE_PULSE1);
            tone(PITCH_C4, BEAT(2), 60, TONE_PULSE2);
            tone(PITCH_E3, BEAT(2), 50, TONE_TRIANGLE);
            stask->timer++;
            break;
        case NOTE_OFF(8):
            tone(PITCH_C5, BEAT(4), 60, TONE_PULSE1);
            tone(PITCH_E4, BEAT(4), 60, TONE_PULSE2);
            tone(PITCH_G3, BEAT(4), 50, TONE_TRIANGLE);
            stask->timer++;
            break;
        case NOTE_OFF(12):
            //tone(PITCH_G4,  27, 60, TONE_PULSE1);
            //tone(PITCH_C5,  27, 60, TONE_PULSE2);
            //tone(PITCH_Gs3, 27, 60, TONE_PULSE1);
            //tone(PITCH_B3,  27, 60, TONE_PULSE2);
            tone(PITCH_G3, BEAT(2), 50, TONE_TRIANGLE);
            stask->timer++;
            break;
        case NOTE_OFF(14):
            return true;
        default:
            stask->timer++;
            return false;
        }
        return false;
    },
    (u8*)&stask);
}