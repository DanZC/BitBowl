#include "title.hpp"
#include "input.hpp"
#include "task.hpp"
#include "util.hpp"
#include "global.hpp"
#include "sound.hpp"
#include "data/graphics.hpp"

#define TITLE_VIEW_MENU     0
#define TITLE_VIEW_RECORDS  1
#define TITLE_VIEW_OPTIONS  2
#define TITLE_VIEW_ERASERC  3
#define TITLE_VIEW_SRECORDS 4

void title_init(UNUSED TitleState* state, UNUSED void* data) 
{
    state->selTimer = 20;
    state->option = 0;
    state->fastSelection = 0;
    state->taskId = TASKID_INVALID;
    state->view = TITLE_VIEW_MENU;
    state->save = nullptr;
}

struct GameSetupTaskData
{
    u8 state;
    u8 taskId;
    u8 kind;
    u8 playerCount;
};

struct GameSetupData
{
    u8 kind;
    u8 playerCount;
};

u8 title_setup_game(u8 kind, u8 playerCount)
{
    GameSetupTaskData tdata{0, TASKID_INVALID, kind, playerCount};
    return task_create([](Task* task, u8* data) {
        memcpy(task->data, data, sizeof(GameSetupTaskData));
    },
    [](Task* task, GlobalData* gdata) -> bool {
        auto tdata = (GameSetupTaskData*)task->data;
        GameSetupData gsd{tdata->kind, tdata->playerCount};
        switch(tdata->state)
        {
        case 0:
            tdata->taskId = task_wait_frames(10);
            tdata->state++;
            fallthrough;
        case 1:
            if(!task_update(tdata->taskId, gdata)) return false;
            task_destroy(tdata->taskId);
            state_set(gdata, STATE_GAME, &gsd);
            task_destroy(task_get_id_from_ptr(task));
            return false;
        case 2:
            return true;
        }
        return false;
    },
    (u8*)&tdata);
}

void title_update(UNUSED TitleState* state, UNUSED GlobalData* gdata) 
{
    if(state->taskId != TASKID_INVALID)
    {
        if(!task_update(state->taskId, gdata)) return;
        task_destroy(state->taskId);
        state->taskId = TASKID_INVALID;
    }

    if(state->save == nullptr) state->save = &gdata->save;

    switch(state->view)
    {
    case TITLE_VIEW_MENU:
        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_1))
        {
            if(state->option == 5)
            {
                state->view = TITLE_VIEW_RECORDS;
                return;
            }
            else if(state->option == 6)
            {
                state->option2 = 0;
                state->optionFlags = save_get_option_flags(state->save);
                state->view = TITLE_VIEW_OPTIONS;
                return;
            }
            else if(state->option == 4)
            {
                state->taskId = title_setup_game(GAME_KIND_SURVIVAL, 1);
                return;
            }
            else 
            {
                state->taskId = title_setup_game(GAME_KIND_NORMAL, state->option + 1);
                return;
            }
        }

        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_DOWN) || GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_UP))
        {
            if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_UP)) 
            {
                if(state->option == 0) state->option = 6;
                else state->option--;
                state->fastSelection = 0;
                state->selTimer = 20;
            } 
            else if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_DOWN)) 
            {
                if(state->option == 6) state->option = 0;
                else state->option++;
                state->fastSelection = 0;
                state->selTimer = 20;
            }
            if(gdata->save.options.flags & OPTION_SOUND)
                sound_move_selection();
        }

        if(state->selTimer > 0)
        {
            state->selTimer--;
        }
        else 
        {
            if(GAMEPAD_PRESSED(PAD(1), BUTTON_UP)) 
            {
                if(state->option == 0) state->option = 6;
                else state->option--;
                state->fastSelection = 1;
                state->selTimer = 10;
                if(gdata->save.options.flags & OPTION_SOUND)
                    sound_move_selection();
            } 
            else if(GAMEPAD_PRESSED(PAD(1), BUTTON_DOWN)) 
            {
                if(state->option == 6) state->option = 0;
                else state->option++;
                state->fastSelection = 1;
                state->selTimer = 10;
                if(gdata->save.options.flags & OPTION_SOUND)
                    sound_move_selection();
            }
            else 
            {
                state->fastSelection = 0;
                state->selTimer = 20;
            }
        }
        break;
    case TITLE_VIEW_RECORDS:
        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_2))
        {
            state->view = TITLE_VIEW_MENU;
            return;
        }
        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_1))
        {
            state->view = TITLE_VIEW_ERASERC;
            return;
        }
        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_RIGHT))
        {
            state->view = TITLE_VIEW_SRECORDS;
            return;
        }
        break;
    case TITLE_VIEW_SRECORDS:
        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_2))
        {
            state->view = TITLE_VIEW_MENU;
            return;
        }
        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_1))
        {
            state->view = TITLE_VIEW_ERASERC;
            return;
        }
        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_LEFT))
        {
            state->view = TITLE_VIEW_RECORDS;
            return;
        }
        break;
    case TITLE_VIEW_ERASERC:
        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_2))
        {
            state->view = TITLE_VIEW_RECORDS;
            return;
        }
        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_1))
        {
            if(gdata->save.options.flags & OPTION_SOUND)
                sound_backspace();
            save_erase_records(state->save);
            state->view = TITLE_VIEW_RECORDS;
            return;
        }
        break;
    case TITLE_VIEW_OPTIONS:
        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_2))
        {
            if(state->optionFlags & OPTION_SOUND)
                sound_backspace();
            state->view = TITLE_VIEW_MENU;
            return;
        }
        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_LEFT))
        {
            state->optionFlags ^= (1 << state->option2);
            if(state->optionFlags & OPTION_SOUND)
                sound_move_selection();
        }
        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_RIGHT))
        {
            state->optionFlags ^= (1 << state->option2);
            if(state->optionFlags & OPTION_SOUND)
                sound_move_selection();
        }
        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_1))
        {
            if(state->optionFlags & OPTION_SOUND)
                sound_enter_char();
            save_set_option_flags(state->save, state->optionFlags); 
            state->view = TITLE_VIEW_MENU;
            return;
        }
        if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_DOWN) || GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_UP))
        {
            if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_UP)) 
            {
                if(state->option2 == 0) state->option2 = 1;
                else state->option2--;
                state->fastSelection = 0;
                state->selTimer = 20;
            } 
            else if(GAMEPAD_JUST_PRESSED(PAD(1), BUTTON_DOWN)) 
            {
                if(state->option2 == 1) state->option2 = 0;
                else state->option2++;
                state->fastSelection = 0;
                state->selTimer = 20;
            }
            if(state->optionFlags & OPTION_SOUND)
                sound_move_selection();
        }

        if(state->selTimer > 0)
        {
            state->selTimer--;
        }
        else 
        {
            if(GAMEPAD_PRESSED(PAD(1), BUTTON_UP)) 
            {
                if(state->option2 == 0) state->option2 = 1;
                else state->option2--;
                state->fastSelection = 1;
                state->selTimer = 10;
                if(state->optionFlags & OPTION_SOUND)
                    sound_move_selection();
            } 
            else if(GAMEPAD_PRESSED(PAD(1), BUTTON_DOWN)) 
            {
                if(state->option2 == 1) state->option2 = 0;
                else state->option2++;
                state->fastSelection = 1;
                state->selTimer = 10;
                if(state->optionFlags & OPTION_SOUND)
                    sound_move_selection();
            }
            else 
            {
                state->fastSelection = 0;
                state->selTimer = 20;
            }
        }
        break;
    }
}

void title_draw(UNUSED TitleState* state)
{
    switch(state->view)
    {
    case TITLE_VIEW_MENU:
    {
        *DRAW_COLORS = 0x4321;
        constexpr i32 xsize = 7*8;
        constexpr i32 ysize = 14*8;
        constexpr i32 ystart = (SCREEN_SIZE/2)-(ysize/2);
        //text("BitBowl", (SCREEN_SIZE/2)-(xsize/2), ystart);
        blit(title, (SCREEN_SIZE/2)-(titleWidth/2), ystart-12, titleWidth, titleHeight, titleFlags);

        *DRAW_COLORS = 0x3;
        constexpr i32 xsize2 = 9*8;
        text(" 1P Game", (SCREEN_SIZE/2)-(xsize2/2), ystart+20);
        text(" 2P Game", (SCREEN_SIZE/2)-(xsize2/2), ystart+20+12);
        text(" 3P Game", (SCREEN_SIZE/2)-(xsize2/2), ystart+20+24);
        text(" 4P Game", (SCREEN_SIZE/2)-(xsize2/2), ystart+20+36);
        text(" Survival",(SCREEN_SIZE/2)-(xsize2/2), ystart+20+48);
        text(" Records", (SCREEN_SIZE/2)-(xsize2/2), ystart+20+60);
        text(" Options", (SCREEN_SIZE/2)-(xsize2/2), ystart+20+72);

        text(">", (SCREEN_SIZE/2)-(xsize2/2), ystart+20+(12*state->option));
        break;
    }
    case TITLE_VIEW_RECORDS:
    {
        *DRAW_COLORS = 0x3;
        constexpr i32 xsize = 19*8;
        constexpr i32 ysize = 14*8;
        constexpr i32 ystart = (SCREEN_SIZE/2)-(ysize/2);
        text("  Bowling records \x85", (SCREEN_SIZE/2)-(xsize/2), ystart);

        constexpr i32 xsize2 = 17*8;
        char buffer[64];
        for(int i = 0; i < 8; i++)
        {
            const SavedScore& score = state->save->hiScores[i];
            strfmt(buffer, "%d. %s - %3d", i+1, score.name, (i32)score.score);
            text(buffer, (SCREEN_SIZE/2)-(xsize2/2), ystart+20+(12*i));
        }

        text("\x81 Back  \x80 Erase All", 0, SCREEN_SIZE-10);
        break;
    }
    case TITLE_VIEW_SRECORDS:
    {
        *DRAW_COLORS = 0x3;
        constexpr i32 xsize = 19*8;
        constexpr i32 ysize = 14*8;
        constexpr i32 ystart = (SCREEN_SIZE/2)-(ysize/2);
        text("\x84 Survival records", (SCREEN_SIZE/2)-(xsize/2), ystart);

        constexpr i32 xsize2 = 17*8;
        char buffer[64];
        for(int i = 0; i < 8; i++)
        {
            const SavedSurvivalRecord& record = state->save->survivalRecords[i];
            strfmt(buffer, "%d. %s - %3d", i+1, record.name, (i32)record.record);
            text(buffer, (SCREEN_SIZE/2)-(xsize2/2), ystart+20+(12*i));
        }

        text("\x81 Back  \x80 Erase All", 0, SCREEN_SIZE-10);
        break;
    }
    case TITLE_VIEW_ERASERC:
    {
        *DRAW_COLORS = 0x3;
        constexpr i32 xsize = 16*8;
        constexpr i32 ysize = 10*8;
        constexpr i32 ystart = (SCREEN_SIZE/2)-(ysize/2);
        text("Are you sure you\nwish to erase all\nbowling records?", (SCREEN_SIZE/2)-(xsize/2), ystart);

        constexpr i32 xsize2 = 12*8;
        text("\x80 Yes   \x81 No", (SCREEN_SIZE/2)-(xsize2/2), ystart + 32);
        break;
    }
    case TITLE_VIEW_OPTIONS:
    {
        *DRAW_COLORS = 0x3;
        constexpr i32 xsize = 7*8;
        constexpr i32 ysize = 8*8;
        constexpr i32 ystart = (SCREEN_SIZE/2)-(ysize/2);
        text("Options", (SCREEN_SIZE/2)-(xsize/2), ystart);

        constexpr i32 xsize2 = 12*8;
        char buffer[64];
        strfmt(buffer, " Sound:  %s", (state->optionFlags & OPTION_SOUND)? "On": "Off");
        text(buffer, (SCREEN_SIZE/2)-(xsize2/2), ystart+20);
        strfmt(buffer, " Music:  %s", (state->optionFlags & OPTION_MUSIC)? "On": "Off");
        text(buffer, (SCREEN_SIZE/2)-(xsize2/2), ystart+20+12);

        text(">", (SCREEN_SIZE/2)-(xsize2/2), ystart+20+(12*state->option2));

        text("\x80 Save  \x81 Cancel", 0, SCREEN_SIZE-10);
        break;
    }
    }
}