#include "game.hpp"
#include "global.hpp"
#include "input.hpp"
#include "task.hpp"
#include "util.hpp"
#include "random.hpp"
#include "sound.hpp"
#include "data/graphics.hpp"
#include "data/metatilesets/bowlingalley_meta.hpp"
#include "data/areas/bowlingalley.hpp"
#include "data/objtypes.hpp"

#define GAME_VIEW_ALLEY     0
#define GAME_VIEW_SCORE     1
#define GAME_VIEW_RESULTS   2
#define GAME_VIEW_YOUREUP   3
#define GAME_VIEW_NAMEENTRY 4
#define GAME_VIEW_SURV_DESC 5
#define GAME_VIEW_STREAK    6
#define GAME_VIEW_SURV_RES  7

#define BALL_STATE_EXTRA        1

static u8 game_init_anim(GameState* state);
static bool game_determine_winner(GameState* gs);
static bool pin_anim(Object* object, u8 state, u16 frameCount);
static void ball_anim(Object* object, u16 skid, u16 frameCount);
static void name_entry_draw(GameState* state);
u8 game_name_entry(GameState* state, u8 scoreIdx, u8 plyrIdx);
i32 check_new_high_score_normal(SaveData* save, u16 winning_score);
i32 check_new_high_score_survival(SaveData* save, u16 final_streak);

static void name_entry_write(GameState* state, char c)
{
    auto nlen = strlen(state->nameBuffer);
    if(nlen == 9)
    {
        state->nameBuffer[nlen-1] = c;
    }
    else 
    {
        state->nameBuffer[nlen] = c;
        state->nameBuffer[nlen+1] = '\0';
    }
}

static void name_entry_backsp(GameState* state)
{
    auto nlen = strlen(state->nameBuffer);
    if(nlen > 0)
    {
        state->nameBuffer[nlen-1] = '\0';
    }
}

static void game_reset_ball_data(GameState* state)
{
    memset((u8*)&state->ballBody.data, sizeof(state->ballBody.data), 0);
    state->ballBody.data.power = 50;
}

static void game_reset_pins(GameState* state)
{
    // Pins
    object_init(&state->objects[0], pin0Coord, 16, 0b101, pins, pinsWidth, pinsFlags);
    // object_init(&state->objects[0], 54, 100, 16, 0b101, pins, pinsWidth, pinsFlags);
    object_init(&state->objects[1], pin1Coord, 16, 0b101, pins, pinsWidth, pinsFlags);
    object_init(&state->objects[2], pin2Coord, 16, 0b101, pins, pinsWidth, pinsFlags);
    object_init(&state->objects[3], pin3Coord, 16, 0b101, pins, pinsWidth, pinsFlags);
    object_init(&state->objects[4], pin4Coord, 16, 0b101, pins, pinsWidth, pinsFlags);
    object_init(&state->objects[5], pin5Coord, 16, 0b101, pins, pinsWidth, pinsFlags);
    object_init(&state->objects[6], pin6Coord, 16, 0b101, pins, pinsWidth, pinsFlags);
    object_init(&state->objects[7], pin7Coord, 16, 0b101, pins, pinsWidth, pinsFlags);
    object_init(&state->objects[8], pin8Coord, 16, 0b101, pins, pinsWidth, pinsFlags);
    object_init(&state->objects[9], pin9Coord, 16, 0b101, pins, pinsWidth, pinsFlags);

    memset((u8*)&state->pinBodies, sizeof(state->pinBodies), 0);

    for(int i = 0; i < 10; i++)
    {
        state->pinBodies[i].x = state->objects[i].x;
        state->pinBodies[i].y = state->objects[i].y;
    }

    state->pinState = 0b1111'1111'11;
}

static void game_sync_pin_states(GameState* state)
{
    for(int i = 0; i < 10; i++)
    {
        state->pinBodies[i].pinState = (state->pinState & (1 << i))? PIN_STANDING: PIN_DOWN;
    }
}

static void game_reset_standing_pins(GameState* state)
{
    if(state->pinState & 0b0000'0000'01)
    {
        object_set_pos(&state->objects[0], pin0Coord);
        state->pinBodies[0].x = state->objects[0].x;
        state->pinBodies[0].y = state->objects[0].y;
    }
    else 
    {
        state->objects[0].sprite.visible = 0;
    }
    if(state->pinState & 0b0000'0000'10)
    {
        object_set_pos(&state->objects[1], pin1Coord);
        state->pinBodies[1].x = state->objects[1].x;
        state->pinBodies[1].y = state->objects[1].y;
    }
    else 
    {
        state->objects[1].sprite.visible = 0;
    }

    if(state->pinState & 0b0000'0001'00)
    {
        object_set_pos(&state->objects[2], pin2Coord);
        state->pinBodies[2].x = state->objects[2].x;
        state->pinBodies[2].y = state->objects[2].y;
    }
    else 
    {
        state->objects[2].sprite.visible = 0;
    }

    if(state->pinState & 0b0000'0010'00)
    {
        object_set_pos(&state->objects[3], pin3Coord);
        state->pinBodies[3].x = state->objects[3].x;
        state->pinBodies[3].y = state->objects[3].y;
    }
    else 
    {
        state->objects[3].sprite.visible = 0;
    }

    if(state->pinState & 0b0000'0100'00)
    {
        object_set_pos(&state->objects[4], pin4Coord);
        state->pinBodies[4].x = state->objects[4].x;
        state->pinBodies[4].y = state->objects[4].y;
    }
    else 
    {
        state->objects[4].sprite.visible = 0;
    }

    if(state->pinState & 0b0000'1000'00)
    {
        object_set_pos(&state->objects[5], pin5Coord);
        state->pinBodies[5].x = state->objects[5].x;
        state->pinBodies[5].y = state->objects[5].y;
    }
    else 
    {
        state->objects[5].sprite.visible = 0;
    }

    if(state->pinState & 0b0001'0000'00)
    {
        object_set_pos(&state->objects[6], pin6Coord);
        state->pinBodies[6].x = state->objects[6].x;
        state->pinBodies[6].y = state->objects[6].y;
    }
    else 
    {
        state->objects[6].sprite.visible = 0;
    }

    if(state->pinState & 0b0010'0000'00)
    {
        object_set_pos(&state->objects[7], pin7Coord);
        state->pinBodies[7].x = state->objects[7].x;
        state->pinBodies[7].y = state->objects[7].y;
    }
    else 
    {
        state->objects[7].sprite.visible = 0;
    }

    if(state->pinState & 0b0100'0000'00)
    {
        object_set_pos(&state->objects[8], pin8Coord);
        state->pinBodies[8].x = state->objects[8].x;
        state->pinBodies[8].y = state->objects[8].y;
    }
    else 
    {
        state->objects[8].sprite.visible = 0;
    }

    if(state->pinState & 0b1000'0000'00)
    {
        object_set_pos(&state->objects[9], pin9Coord);
        state->pinBodies[9].x = state->objects[9].x;
        state->pinBodies[9].y = state->objects[9].y;
    }
    else 
    {
        state->objects[9].sprite.visible = 0;
    }
}

void game_reset_player(GameState* state)
{
    state->playerState = 0;
    object_init(&state->objects[11],personCoord,24, 0b011, person, personWidth, personFlags);
}

struct GameSetupData
{
    u8 kind;
    u8 playerCount;
};

void game_init(GameState* state, void* data)
{
    state->camPosX = 0;
    state->camPosY = 0;
    state->taskId = 0;

    state->playerState = 0;
    state->ballState = 0;
    state->playerCount = (data == nullptr)? 1: ((GameSetupData*)data)->playerCount;
    state->gameKind = (data == nullptr)? GAME_KIND_NORMAL: ((GameSetupData*)data)->kind;

    if(state->gameKind == GAME_KIND_NORMAL)
    {
        state->frameNum = 0;
        state->throwNum = 0;
        state->view = GAME_VIEW_YOUREUP;
    }
    else 
    {
        state->streak = 0;
        state->sparePinCount = 1;
        state->view = GAME_VIEW_SURV_DESC;
    }
    
    state->turn = 0;

    for(int i = 0; i < (int)state->playerCount; i++)
        init_player_data(&state->score[i]);

    // Pins
    game_reset_pins(state);

    // Ball
    object_init(&state->objects[10],ballCoord,   8, 0b101, ball2, ball2Width, ball2Flags);
    state->objects[10].sprite.visible = 0;

    // Person
    game_reset_player(state);

    game_reset_ball_data(state);

    state->taskId = game_init_anim(state);
    //state->winningScore = 1;
    //state->winningPlayer = 0;
    //state->taskId = game_name_entry(state, 0, 0);
}

#define STATE_FLAG_STRIKE 1
#define STATE_FLAG_SPARE  2
#define STATE_FLAG_GUTTER 4

// Moves to the next frame.
static void game_advance_frame(GameState* state)
{
    state->frameNum++;
}

// Moves to the next players turn or the next frame if all players have 
// had a turn.
static void game_advance_turn(GameState* state)
{
    state->turn++;
    state->ballState = 0;
    game_reset_pins(state);
    game_reset_ball_data(state);
    if(state->turn >= state->playerCount)
    {
        state->turn = 0;
        game_advance_frame(state);
    }
}

static void game_advance_throw(GameState* state)
{
    state->throwNum++;

    // All frames besides the final frame get two throws.
    if(state->frameNum < 9 && state->throwNum == 2)
    {
        state->throwNum = 0;
        game_advance_turn(state);
    }

    // The final frame gets three throws.
    else if(state->frameNum == 9 && state->throwNum == 3)
    {
        state->throwNum = 0;
        game_advance_turn(state);
    }
    else 
    {
        game_reset_standing_pins(state);
    }
}

// Handles turning rotation and advancing the game state after each throw.
static void game_handle_advance_state(GameState* state, u8 flags)
{
    // If a player got a strike on throw 1 of any frame except the final one,
    // skip the second throw.
    if((flags & STATE_FLAG_STRIKE) && state->throwNum == 0 && state->frameNum < 9)
    {
        game_advance_turn(state);
    }

    // On the final frame, if the player gets a strike, reset all pins.
    else if((flags & STATE_FLAG_STRIKE) && state->frameNum == 9)
    {
        game_reset_pins(state);
        state->ballState |= BALL_STATE_EXTRA;
        game_advance_throw(state);
    }

    // On the final frame, if the player gets a spare on their second throw, 
    // reset all pins.
    else if((flags & STATE_FLAG_SPARE) && state->frameNum == 9 && state->throwNum == 1)
    {
        game_reset_pins(state);
        state->ballState |= BALL_STATE_EXTRA;
        game_advance_throw(state);
    }

    // If the player did not get a strike or spare by throw 2 of their final frame,
    // skip the third throw.
    else if(((flags & (STATE_FLAG_SPARE)) == 0) 
        && (state->ballState & BALL_STATE_EXTRA) == 0
        && state->frameNum == 9 
        && state->throwNum == 1)
    {
        game_advance_turn(state);
    }
    else 
    {
        game_advance_throw(state);
    }
}

static u16 game_generate_random_pins(GameState* state)
{
    u16 output = 0;
    for(int i = 0; i < (int)state->sparePinCount; i++)
    {
        u16 pin = (u16)random_int_range(0, 9);
        output |= (u16)(1 << pin);
    }
    return output;
}

static void game_handle_advance_survival_streak(GameState* state)
{
    state->streak++;
    game_reset_pins(state);
    state->pinState = game_generate_random_pins(state);
    game_sync_pin_states(state);
    game_reset_standing_pins(state);
    game_reset_player(state);
}

struct GameInitAnimTaskData
{
    u8 state;
    u8 taskId;
    GameState* game;
};

static u8 game_init_anim(GameState* state)
{
    return task_create([](Task* task, u8* data) {
        auto tdata = (GameInitAnimTaskData*)task->data;
        tdata->state = 0;
        tdata->taskId = TASKID_INVALID;
        tdata->game = (GameState*)data;
    },
    [](Task* task, GlobalData* gdata) -> bool {
        auto tdata = (GameInitAnimTaskData*)task->data;
        auto game = tdata->game;
        switch(tdata->state)
        {
        case 0:
            if(game->gameKind == GAME_KIND_SURVIVAL)
            {
                if(game->streak == 0)
                {
                    tdata->taskId = task_wait_input(BUTTON_1, game->turn);
                    game->pinState = game_generate_random_pins(game);
                    game_sync_pin_states(game);
                    game_reset_standing_pins(game);
                }
                tdata->state++;
                return false;
            }
            if(game->throwNum == 0)
            {
                game->view = GAME_VIEW_YOUREUP;
                tdata->taskId = task_wait_input(BUTTON_1, game->turn);
                //tracef("%d - You're up", game->throwNum);
                tdata->state++;
                return false;
            }
            else 
            {
                //tracef("%d", game->throwNum);
                tdata->state++;
            }
            fallthrough;
        case 1:
            if(tdata->taskId != TASKID_INVALID)
            {
                if(!task_update(tdata->taskId, gdata)) return false;
                task_destroy(tdata->taskId);
                game->view = GAME_VIEW_ALLEY;
            }
            tdata->taskId = task_wait_frames(60);
            tdata->state++;
            fallthrough;
        case 2:
            if(!task_update(tdata->taskId, gdata)) return false;
            task_destroy(tdata->taskId);
            tdata->state++;
            fallthrough;
        case 3:
            tdata->game->camPosY++;
            if(GAMEPAD_PRESSED(tdata->game->turn, BUTTON_2)) tdata->game->camPosY++;
            if(tdata->game->camPosY < (17*16)-SCREEN_SIZE) return false;
            tdata->game->camPosY = (17*16)-SCREEN_SIZE;
            tdata->taskId = task_wait_frames(5);
            tdata->state++;
            fallthrough;
        case 4:
            if(!task_update(tdata->taskId, gdata)) return false;
            task_destroy(tdata->taskId);
            tdata->state++;
            fallthrough;
        case 5:
            return true;
        default:
            return false;
        }
        return false;
    },
    (u8*)state);
}

struct GameThrowAnimTaskData
{
    u8 state;
    u8 taskId;
    u8 frameCount;
    GameState* game;
};

static u8 game_throw_anim(GameState* state)
{
    return task_create([](Task* task, u8* data) {
        auto tdata = (GameThrowAnimTaskData*)task->data;
        tdata->state = 0;
        tdata->taskId = TASKID_INVALID;
        tdata->game = (GameState*)data;
    },
    [](Task* task, GlobalData* gdata) -> bool {
        auto tdata = (GameThrowAnimTaskData*)task->data;
        auto game = tdata->game;
        auto bbody = &game->ballBody;
        auto bball = &game->objects[10];
        auto plyr  = &game->objects[11];
        u16 pinsLeft;
        u8 pinHitCount;
        u8 throwFlags;
        i32 advanceCount;

        bool stillAnimating = false;
        switch(tdata->state)
        {
        case 0:
            if(tdata->game->playerState < 50)
            {
                tdata->game->playerState++;
                return false;
            }
            tdata->state++;
            fallthrough;
        case 1:
            bball->sprite.visible = 1;
            bball->y = plyr->y + 7;
            bball->x = plyr->x + 4;
            game->ballBody.y = (f32)bball->y;
            game->ballBody.x = (f32)bball->x;

            if(gdata->save.options.flags & OPTION_SOUND)
                sound_throw_ball();

            // Simulate throwing power + sway
            // game->ballBody.data.power = (u16)random_int_range(40, 90);
            game->ballBody.data.skid = 30;
            // game->ballBody.data.sway = (i16)((GAMEPAD_PRESSED(game->turn, BUTTON_RIGHT) > 0)-(GAMEPAD_PRESSED(game->turn, BUTTON_LEFT) > 0)) *2;

            tdata->frameCount = 0;

            tdata->state++;
            fallthrough;
        case 2:
            advanceCount = (GAMEPAD_PRESSED(game->turn, BUTTON_2))? 1: 0;
            do {
                do_ball_physics(&game->ballBody, tdata->frameCount++);
                handle_ball_collisions(&game->ballBody, game->pinBodies, 10, gdata->save.options.flags);
                handle_pin_collisions(game->pinBodies, 10);
                if(tdata->frameCount > 120) tdata->frameCount -= 120;
                
                bball->y = (i16)roundi(game->ballBody.y);
                bball->x = (i16)roundi(game->ballBody.x);

                ball_anim(bball, bbody->data.skid, bbody->frameCount++);
                if(bbody->data.skid > 0) bbody->frameCount = 0;

                for(int i = 0; i < 10; i++)
                {
                    auto pBody = &game->pinBodies[i];

                    if(pBody->pinState & PIN_STATE_CHANGED)
                    {
                        switch(pBody->pinState & 0b111)
                        {
                        case PIN_ROCKING:
                        case PIN_FALLING:
                            if(pBody->dx < 0.f)
                                game->objects[i].sprite.flags |= BLIT_FLIP_X;
                            else
                                game->objects[i].sprite.flags &= (u8)~BLIT_FLIP_X;
                            pBody->pinState &= 0b111;
                            pBody->frameCount = 0; 
                            fallthrough;
                        default:
                            pBody->pinState &= 0b111;
                            pBody->frameCount = 0; 
                            break;
                        }
                    }

                    if(pBody->pinState == PIN_FALLING || pBody->pinState == PIN_ROCKING) 
                        stillAnimating = true;

                    if(pin_anim(&game->objects[i], pBody->pinState, pBody->frameCount))
                    {
                        if(pBody->pinState == PIN_ROCKING) pBody->pinState = PIN_STANDING;
                        else pBody->pinState++;
                        pBody->frameCount = 0;
                    }
                    else 
                    {
                        pBody->frameCount++;
                    }

                    game->objects[i].x = (i16)roundi(pBody->x);
                    game->objects[i].y = (i16)roundi(pBody->y);
                }

                // If the ball is in the gutter, the player can skip the animation.
                if((bball->x <= 48 || bball->x >= 104) && GAMEPAD_JUST_PRESSED(game->turn, BUTTON_1))
                {
                    bball->sprite.visible = 0;
                    tdata->game->camPosY = 0;
                    tdata->taskId = task_wait_frames(15);
                    tdata->state++;
                    return false;
                }
            
                if(bball->y > 10)
                {
                    if(bball->y - tdata->game->camPosY < (SCREEN_SIZE / 2) && tdata->game->camPosY > 0)
                    {
                        tdata->game->camPosY = bball->y - (SCREEN_SIZE / 2);
                    }
                    continue;
                }
                else 
                {
                    bball->sprite.visible = 0;
                }
            
            } while(advanceCount-- > 0);

            if(bball->y > 10 || stillAnimating) return false;

            tdata->taskId = task_wait_frames(60);
            tdata->state++;
            fallthrough;
        case 3:
            if(!task_update(tdata->taskId, gdata)) return false;
            task_destroy(tdata->taskId);

            throwFlags = 0;
            pinsLeft = check_pins(game->pinBodies);
            pinHitCount = (u8)popcount(game->pinState ^ pinsLeft);

            if(pinHitCount == 10 
            && (game->throwNum == 0 || (game->ballState & BALL_STATE_EXTRA)))
            {
                throwFlags |= STATE_FLAG_STRIKE;
            }
            else if(pinsLeft == 0)
            {
                throwFlags |= STATE_FLAG_SPARE;
            }
            else if(bball->x <= 48 || bball->x >= 104)
            {
                throwFlags |= STATE_FLAG_GUTTER;
            }
            if(gdata->save.options.flags & OPTION_MUSIC)
            {
                if(throwFlags & STATE_FLAG_STRIKE)
                {
                    tdata->taskId = sound_me_strike();
                    tdata->state++;
                    return false;
                }
                else if(throwFlags & STATE_FLAG_SPARE)
                {
                    tdata->taskId = sound_me_spare();
                    tdata->state++;
                    return false;
                }
                else if(throwFlags & STATE_FLAG_GUTTER)
                {
                    tdata->taskId = sound_me_gutter();
                    tdata->state++;
                    return false;
                }
                else if(pinHitCount == 0)
                {
                    tdata->taskId = sound_me_gutter();
                    tdata->state++;
                    return false;
                }
            }

            if(game->gameKind == GAME_KIND_SURVIVAL)
            {
                if(throwFlags & STATE_FLAG_SPARE)
                {
                    game_handle_advance_survival_streak(game);
                    tdata->taskId = task_wait_input(BUTTON_1);
                    game->view = GAME_VIEW_STREAK;
                }
                else
                {
                    tdata->taskId = task_wait_input(BUTTON_1);
                    game->view = GAME_VIEW_SURV_RES;
                    game->finalStreak = game->streak;
                }
            }
            else 
            {
                game->score[game->turn].frames[game->frameNum].throws[game->throwNum] = 
                    (throwFlags & STATE_FLAG_GUTTER)
                        ? THROW_GUTTER
                        : pinHitCount;
                game->pinState = pinsLeft;
                tdata->taskId = task_wait_input(BUTTON_1, game->turn);
                game->viewFrame = (game->frameNum < 2)? 0: game->frameNum - 2;
                game_handle_advance_state(game, throwFlags);
                game->view = GAME_VIEW_SCORE;
            }
            tdata->state += 2;
            fallthrough;
        case 5:
            if(!task_update(tdata->taskId, gdata)) return false;
            task_destroy(tdata->taskId);
            if(game->gameKind == GAME_KIND_SURVIVAL)
            {
                if(game->view == GAME_VIEW_SURV_RES)
                {
                    i32 fscore = check_new_high_score_survival(&gdata->save, game->finalStreak);
                    if(fscore != -1)
                    {
                        tdata->taskId = game_name_entry(game, (u8)fscore, 0);
                        tdata->state++;
                        return false;
                    }
                    else 
                    {
                        state_set(gdata, STATE_TITLE, nullptr);
                        task_destroy(task_get_id_from_ptr(task));
                    }
                }
                game->view = GAME_VIEW_ALLEY;
                tdata->taskId = game_init_anim(tdata->game);
            }
            else if(game->frameNum >= 10)
            {
                game->view = GAME_VIEW_RESULTS;
                tdata->taskId = task_wait_input(BUTTON_1, 0);
                tdata->state++;
                return false;
            }
            else
            {
                game->view = (game->throwNum == 0)? GAME_VIEW_YOUREUP: GAME_VIEW_ALLEY;
                game_reset_player(tdata->game);
                tdata->taskId = game_init_anim(tdata->game);
            }
            tdata->state++;
            fallthrough;
        case 6:
            if(!task_update(tdata->taskId, gdata)) return false;
            task_destroy(tdata->taskId);
            if(game->gameKind == GAME_KIND_NORMAL
            && game->frameNum >= 10 
            && game_determine_winner(game))
            {
                i32 fscore = check_new_high_score_normal(&gdata->save, game->winningScore);
                if(fscore != -1)
                {
                    tdata->taskId = game_name_entry(game, (u8)fscore, game->winningPlayer);
                    tdata->state++;
                    return false;
                }
            }
            return true;
        case 7:
            if(!task_update(tdata->taskId, gdata)) return false;
            task_destroy(tdata->taskId);
            return true;
        case 4:
            if(!task_update(tdata->taskId, gdata)) return false;
            task_destroy(tdata->taskId);

            throwFlags = 0;
            pinsLeft = check_pins(game->pinBodies);
            pinHitCount = (u8)popcount(game->pinState ^ pinsLeft);

            if(pinHitCount == 10 
            && (game->throwNum == 0 || (game->ballState & BALL_STATE_EXTRA)))
            {
                throwFlags |= STATE_FLAG_STRIKE;
            }
            else if(pinsLeft == 0)
            {
                throwFlags |= STATE_FLAG_SPARE;
            }
            else if(bball->x <= 48 || bball->x >= 104)
            {
                throwFlags |= STATE_FLAG_GUTTER;
            }

            if(game->gameKind == GAME_KIND_SURVIVAL)
            {
                if(throwFlags & STATE_FLAG_SPARE)
                {
                    game_handle_advance_survival_streak(game);
                    tdata->taskId = task_wait_input(BUTTON_1);
                    game->view = GAME_VIEW_STREAK;
                    tdata->state++;
                    return false;
                }
                else
                {
                    tdata->taskId = task_wait_input(BUTTON_1);
                    game->view = GAME_VIEW_SURV_RES;
                    game->finalStreak = game->streak;
                    tdata->state++;
                    return false;
                }
            }

            game->score[game->turn].frames[game->frameNum].throws[game->throwNum] = 
                (throwFlags & STATE_FLAG_GUTTER)
                    ? THROW_GUTTER
                    : pinHitCount;
            game->pinState = pinsLeft;
            tdata->taskId = task_wait_input(BUTTON_1, game->turn);
            game->viewFrame = (game->frameNum < 2)? 0: game->frameNum - 2;
            game_handle_advance_state(game, throwFlags);
            game->view = GAME_VIEW_SCORE;
            tdata->state++;
            return false;
        default:
            return false;
        }
        return false;
    },
    (u8*)state);
}

struct NameEntryTaskData
{
    u8 scoreIdx;
    u8 plyrIdx;
    u8 curPos;
    u8 state;
    GameState* game;
};

static const char sNameEntryCharMap[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ.'      ";

u8 game_name_entry(GameState* state, u8 scoreIdx, u8 plyrIdx)
{
    NameEntryTaskData dat{scoreIdx, plyrIdx, 0, 0, state};
    return task_create([](Task* task, u8* data){
        memcpy(task->data, data, sizeof(NameEntryTaskData));
    },
    [](Task* task, UNUSED GlobalData* gdata) -> bool {
        auto tdata = (NameEntryTaskData*)task->data;
        auto game = tdata->game;
        switch(tdata->state)
        {
        case 0:
            game->view = GAME_VIEW_NAMEENTRY;
            strcpy(game->nameBuffer, "");
            game->cursorOption = 0x00;
            game->cursorFrame = 0;
            tdata->state++;
            return false; // wait frame
        case 1:
            game->cursorFrame++;

            if(GAMEPAD_JUST_PRESSED(tdata->plyrIdx, BUTTON_DOWN))
            {
                u8 yPos = ((game->cursorOption & 0xF0) >> 4);
                if(yPos == 5) yPos = 0;
                else yPos++;
                
                game->cursorOption &= 0x0F;
                game->cursorOption |= (u8)(yPos << 4);

                if(gdata->save.options.flags & OPTION_SOUND)
                    sound_move_selection();
            }

            if(GAMEPAD_JUST_PRESSED(tdata->plyrIdx, BUTTON_UP))
            {
                u8 yPos = ((game->cursorOption & 0xF0) >> 4);
                if(yPos == 0) yPos = 5;
                else yPos--;
                game->cursorOption &= 0x0F;
                game->cursorOption |= (u8)(yPos << 4);

                if(gdata->save.options.flags & OPTION_SOUND)
                    sound_move_selection();
            }

            if(GAMEPAD_JUST_PRESSED(tdata->plyrIdx, BUTTON_LEFT))
            {
                u8 xPos = game->cursorOption & 0xF;
                if((game->cursorOption >> 4) == 5)
                {
                    if(xPos >= 3) xPos = 2;
                    else xPos = 4;
                }
                else 
                {
                    if(xPos == 0) xPos = 5;
                    else xPos--;
                }
                game->cursorOption &= 0xF0;
                game->cursorOption |= (u8)(xPos);

                if(gdata->save.options.flags & OPTION_SOUND)
                    sound_move_selection();
            }

            if(GAMEPAD_JUST_PRESSED(tdata->plyrIdx, BUTTON_RIGHT))
            {
                u8 xPos = game->cursorOption & 0xF;
                if((game->cursorOption >> 4) == 5)
                {
                    if(xPos < 3) xPos = 4;
                    else xPos = 2;
                }
                else 
                {
                    if(xPos == 5) xPos = 0;
                    else xPos++;
                }
                game->cursorOption &= 0xF0;
                game->cursorOption |= (u8)(xPos);

                if(gdata->save.options.flags & OPTION_SOUND)
                    sound_move_selection();
            }

            if(GAMEPAD_JUST_PRESSED(tdata->plyrIdx, BUTTON_1))
            {
                u8 xPos =  game->cursorOption & 0x0F;
                u8 yPos = (game->cursorOption & 0xF0) >> 4;
                if(yPos == 5)
                {
                    if(xPos < 3)
                    {
                        // BKSP
                        if(gdata->save.options.flags & OPTION_SOUND)
                            sound_backspace();
                        name_entry_backsp(game);
                    }
                    else 
                    {
                        // END
                        if(game->gameKind == GAME_KIND_SURVIVAL)
                        {
                            save_store_survival_record(&gdata->save, game->nameBuffer, game->finalStreak);
                        }
                        else 
                        {
                            save_store_score(&gdata->save, game->nameBuffer, game->winningScore);
                        }
                        state_set(gdata, STATE_TITLE, nullptr);
                        task_destroy(task_get_id_from_ptr(task));
                        return false;
                    }
                }
                else
                {
                    if(gdata->save.options.flags & OPTION_SOUND)
                        sound_enter_char();
                    name_entry_write(game, sNameEntryCharMap[yPos*6 + xPos]);
                }
            }

            if(GAMEPAD_JUST_PRESSED(tdata->plyrIdx, BUTTON_2))
            {
                // BKSP
                if(gdata->save.options.flags & OPTION_SOUND)
                    sound_backspace();
                name_entry_backsp(game);
            }

            return false;
        }
        return false;
    },
    (u8*)&dat);
}

void game_update(GameState* state, UNUSED GlobalData* gdata)
{
    if(state->taskId != TASKID_INVALID)
    {
        if(!task_update(state->taskId, gdata)) return;
        task_destroy(state->taskId);
        state->taskId = TASKID_INVALID;
        return;
    }

    if(state->gameKind == GAME_KIND_NORMAL && state->frameNum >= 10)
    {
        state_set(gdata, STATE_TITLE, nullptr);
        return;
    }

    if(GAMEPAD_JUST_PRESSED(PAD(state->turn+1), BUTTON_1))
    {
        // Snap camera back to player.
        state->camPosY = (17*16)-SCREEN_SIZE;

        state->taskId = game_throw_anim(state);
        return;
    }

    if(state->meterDir > 0)
    {
        state->ballBody.data.power++;
        if(state->ballBody.data.power >= 100)
        {
            state->meterDir = -1;
        }
    }
    else 
    {
        state->ballBody.data.power--;
        if(state->ballBody.data.power <= 20)
        {
            state->meterDir = 1;
        }
    }

    if(!GAMEPAD_PRESSED(state->turn, BUTTON_2))
    {
        /*
        if(GAMEPAD_PRESSED(state->turn, BUTTON_UP))
        {
            state->ballBody.data.power++;
        }
        if(GAMEPAD_PRESSED(state->turn, BUTTON_DOWN))
        {
            state->ballBody.data.power--;
        }
        */
        if(GAMEPAD_JUST_PRESSED(state->turn, BUTTON_LEFT))
        {
            state->ballBody.data.sway--;
        }
        if(GAMEPAD_JUST_PRESSED(state->turn, BUTTON_RIGHT))
        {
            state->ballBody.data.sway++;
        }

        state->ballBody.data.sway = (i16)clamp((i32)state->ballBody.data.sway, -2, 2);
        state->ballBody.data.power = (u16)clamp((i32)state->ballBody.data.power, 10, 100);
    }
    else 
    {
        if(GAMEPAD_PRESSED(PAD(state->turn+1), BUTTON_DOWN))
        {
            if(state->camPosY < (17*16)-SCREEN_SIZE)
            {
                state->camPosY++;
            }
        }
        if(GAMEPAD_PRESSED(PAD(state->turn+1), BUTTON_UP))
        {
            if(state->camPosY > 0)
            {
                state->camPosY--;
            }
        }
    }
}

static void metatile_draw(i32 x, i32 y, u8 idx, const u8* mts, const u8* image, u32 imageStride)
{
    const u8* mtsi = &mts[4*idx];
    blitSub(image, x+0, y+0, 8, 8, 0, imageStride * mtsi[0], imageStride, BLIT_2BPP);
    blitSub(image, x+8, y+0, 8, 8, 0, imageStride * mtsi[1], imageStride, BLIT_2BPP);
    blitSub(image, x+0, y+8, 8, 8, 0, imageStride * mtsi[2], imageStride, BLIT_2BPP);
    blitSub(image, x+8, y+8, 8, 8, 0, imageStride * mtsi[3], imageStride, BLIT_2BPP);
}

static void bowling_alley_back_draw(UNUSED GameState* state)
{
    *DRAW_COLORS = 0x4321;
    i32 ystart = (state->camPosY / 16);
    i32 yoffset = state->camPosY % 16;
    i32 yend = ystart + (SCREEN_SIZE / 16) + 1;
    for(int yy = ystart; yy < yend; yy++)
    {
        for(int xx = 0; xx < 10; xx++)
        {
            i32 offset = (yy*bowlingalleyAreaWidth)+xx;
            u8 midx = (offset & 1)? (u8)(bowlingalley_area_data[offset >> 1] & 0xF): (u8)(bowlingalley_area_data[offset >> 1] >> 4);
            //u8 midx = (u8)(((yy*bowlingalleyAreaWidth)+xx) & 0xF);
            metatile_draw((xx * 16), ((yy - ystart) * 16) - yoffset, midx, bowlingalley_meta, bowlingalley, bowlingalleyWidth);
        }
    }
}

static void person_anim(Object* object, u8 state)
{
    if(state == 0)
    {
        object->sprite.frame = 0;
        object->y = (personCoord);
    }
    else if(state < 30)
    {
        object->sprite.frame = 1 + ((state / 5) & 1);
        object->y = (personCoord) - (state / 2);
    }
    else if(state < 50)
    {
        object->sprite.frame = 3 + (state - 30) / 10;
        object->y = (personCoord) - 15;
    }
    else 
    {
        object->sprite.frame = 5;
        object->y = (personCoord) - 15;
    }
}

static void ball_anim(Object* object, u16 skid, u16 frameCount)
{
    if(skid > 0)
    {
        object->sprite.frame = 0;
    }
    else
    {
        object->sprite.frame = (frameCount >> 1) & 0xF;
    }
}

static bool pin_anim(Object* object, u8 state, u16 frameCount)
{
    switch(state)
    {
    case PIN_STANDING:
        object->sprite.frame = 0;
        break;
    case PIN_DOWN:
        object->sprite.frame = 4;
        break;
    case PIN_ROCKING:
        if(frameCount == 64)
        {
            object->sprite.frame = 0;
            return true;
        }
        switch((frameCount >> 1) & 3) {
        case 2:
            object->sprite.flags |= BLIT_FLIP_X;
            object->sprite.frame = 1;
            break;
        case 0:
            object->sprite.flags &= (u8)~BLIT_FLIP_X;
            object->sprite.frame = 1;
            break;
        case 3:
        case 1:
            object->sprite.frame = 0;
            break;
        }
        break;
    case PIN_FALLING:
        if(frameCount < 5)
        {
            object->sprite.frame = 1;
        }
        else if(frameCount < 10)
        {
            object->sprite.frame = 2;
        }
        else if(frameCount < 15)
        {
            object->sprite.frame = 3;
        }
        else 
        {
            object->sprite.frame = 4;
            return true;
        }
        break;
    }
    return false;
}

i32 check_new_high_score_normal(SaveData* save, u16 winning_score)
{
    for(int i = 0; i < 8; i++)
    {
        if(save->hiScores[i].score < winning_score)
        {
            return i;
        }
    }
    return -1;
}

i32 check_new_high_score_survival(SaveData* save, u16 final_streak)
{
    for(int i = 0; i < 8; i++)
    {
        if(save->survivalRecords[i].record < final_streak)
        {
            return i;
        }
    }
    return -1;
}

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
    if(t == THROW_GUTTER) return '_';
    if(t == THROW_NULL) return ' ';
    if(ti == 1 && ((t1 & 0x7f) + t2 == 10)) return '/';
    if(t == 10) return 'X';
    return (char)('0' + t);
}

void frame_draw(i32 x, i32 y, i32 f, const PlayerData* pd)
{
    if(f > 9) return;

    *DRAW_COLORS = 0x30;
    blitSub(frame, x+0,  y+0,  frameSIZE, frameCORNER,  frameWidth, BLIT_1BPP);
    blitSub(frame, x+8,  y+0,  frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
    blitSub(frame, x+16, y+0,  frameSIZE, frameT,       frameWidth, BLIT_1BPP);
    blitSub(frame, x+24, y+0,  frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
    if(f == 9)
    {
        blitSub(frame, x+32, y+0,  frameSIZE, frameT,       frameWidth, BLIT_1BPP | BLIT_FLIP_X);
        blitSub(frame, x+40, y+0,  frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
        blitSub(frame, x+48, y+0,  frameSIZE, frameCORNER,  frameWidth, BLIT_1BPP | BLIT_FLIP_X);

        blitSub(frame, x+48, y+8,  frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP | BLIT_ROTATE);

        blitSub(frame, x+32, y+16, frameSIZE, frameT,       frameWidth, BLIT_1BPP | BLIT_FLIP_Y);
        blitSub(frame, x+40, y+16, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
        blitSub(frame, x+48, y+16, frameSIZE, frameT,       frameWidth, BLIT_1BPP | BLIT_ROTATE | BLIT_FLIP_Y);

        blitSub(frame, x+48, y+24, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP | BLIT_ROTATE);

        blitSub(frame, x+32, y+32, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
        blitSub(frame, x+40, y+32, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
        blitSub(frame, x+48, y+32, frameSIZE, frameCORNER,  frameWidth, BLIT_1BPP | BLIT_ROTATE | BLIT_FLIP_Y);
    }
    else
    {
        blitSub(frame, x+32, y+0,  frameSIZE, frameCORNER,  frameWidth, BLIT_1BPP | BLIT_FLIP_X);

        blitSub(frame, x+32, y+16, frameSIZE, frameT,       frameWidth, BLIT_1BPP | BLIT_ROTATE | BLIT_FLIP_Y);

        blitSub(frame, x+32, y+24, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP | BLIT_ROTATE);

        blitSub(frame, x+32, y+32, frameSIZE, frameCORNER,  frameWidth, BLIT_1BPP | BLIT_ROTATE | BLIT_FLIP_Y);
    }

    blitSub(frame, x+0,  y+8,  frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP | BLIT_ROTATE);
    blitSub(frame, x+16, y+8,  frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP | BLIT_ROTATE);
    blitSub(frame, x+32, y+8,  frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP | BLIT_ROTATE);

    blitSub(frame, x+0,  y+16, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP | BLIT_ROTATE);
    //blitSub(frame, x+8,  y+16, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
    blitSub(frame, x+16, y+16, frameSIZE, frameCORNER,  frameWidth, BLIT_1BPP | BLIT_FLIP_Y);
    blitSub(frame, x+24, y+16, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
    //blitSub(frame, x+32, y+16, frameSIZE, frameT,       frameWidth, BLIT_1BPP | BLIT_ROTATE | BLIT_FLIP_Y);

    blitSub(frame, x+0,  y+24, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP | BLIT_ROTATE);
    //blitSub(frame, x+32, y+24, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP | BLIT_ROTATE);

    blitSub(frame, x+0,  y+32, frameSIZE, frameCORNER,  frameWidth, BLIT_1BPP | BLIT_ROTATE);
    blitSub(frame, x+8,  y+32, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
    blitSub(frame, x+16, y+32, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
    blitSub(frame, x+24, y+32, frameSIZE, frameSTRAGHT, frameWidth, BLIT_1BPP);
    //blitSub(frame, x+32, y+32, frameSIZE, frameCORNER,  frameWidth, BLIT_1BPP | BLIT_ROTATE | BLIT_FLIP_Y);

    *DRAW_COLORS = 0x3;
    char buffer[64];
    auto score = score_player(pd, f + 1);
    const Frame& fr = pd->frames[f];
    if(score >= 0)
    {
        if(f == 9)
        {
            strfmt(buffer, "\n %c %c %c \n\n   %3d \n\n", 
                get_char_from_throw(fr.throws[0], fr.throws[1], 0), 
                get_char_from_throw(fr.throws[0], fr.throws[1], 1),
                get_char_from_throw(fr.throws[2], 0, 0), 
                score);
        }
        else 
        {
            strfmt(buffer, "\n %c %c \n\n %3d \n\n", 
                get_char_from_throw(fr.throws[0], fr.throws[1], 0), 
                get_char_from_throw(fr.throws[0], fr.throws[1], 1), 
                score);
        }
    }
    else 
    {
        strfmt(buffer, "\n %c %c \n\n     \n\n", 
            get_char_from_throw(fr.throws[0], fr.throws[1], 0), 
            get_char_from_throw(fr.throws[0], fr.throws[1], 1));
    }
    text(buffer, x, y);
}

void stat_screen_draw(const GameState* gs, u8 f)
{
    *DRAW_COLORS = 0x3;
    char buffer[32];
    for(i32 ff = 1; ff < 5 && ff+(i32)f < 11; ff++)
    {
        strfmt(buffer, "%d", (i32)f+ff);
        text(buffer, 8+(32*ff), 0);
    }
    /*
    strfmt(buffer, "%d", (i32)f+2);
    text(buffer, 64+8, 0);
    strfmt(buffer, "%d", (i32)f+3);
    text(buffer, 96+8, 0);
    strfmt(buffer, "%d", (i32)f+4);
    text(buffer, 128+8, 0);
    */
    for(u8 i = 0; i < gs->playerCount; i++)
    {
        strfmt(buffer, "P%c", (char)('1' + (i)));
        text(buffer, 6, 24+(i * 32));
        frame_draw(32-8,  8+(i * 32),  f,   &gs->score[i]);
        frame_draw(64-8,  8+(i * 32),  f+1, &gs->score[i]);
        frame_draw(96-8,  8+(i * 32),  f+2, &gs->score[i]);
        frame_draw(128-8, 8+(i * 32),  f+3, &gs->score[i]);
    }

    text("Press \x80 to continue.", (SCREEN_SIZE / 2) - ((19*8)/2), SCREEN_SIZE-10);
}

static bool game_determine_winner(GameState* gs)
{
    i32 scores[4];
    i32 highestScore = -2;
    i32 winners[4];
    i32 winnerCount = 0;

    for(u8 i = 0; i < gs->playerCount; i++)
    {
        scores[i] = score_player(&gs->score[i], 10);
        if(scores[i] > highestScore)
        {
            winners[0] = i;
            highestScore = scores[i];
            winnerCount = 1;
        }
        else if(scores[i] == highestScore)
        {
            winners[winnerCount] = i;
            winnerCount++;
        }
    }

    if(winnerCount == 1 && scores[winners[0]] >= 0)
    {
        gs->winningPlayer = (u8)winners[0];
        gs->winningScore  = (u16)scores[gs->winningPlayer];
        return true;
    }
    else 
    {
        gs->winningPlayer = 255;
        gs->winningScore  =   0;
        return false;
    }
}

void results_draw(const GameState* gs)
{
    *DRAW_COLORS = 0x3;
    char buffer[32];

    i32 scores[4];
    i32 highestScore = -2;
    i32 winners[4];
    i32 winnerCount = 0;

    for(u8 i = 0; i < gs->playerCount; i++)
    {
        scores[i] = score_player(&gs->score[i], 10);
        if(scores[i] > highestScore)
        {
            winners[0] = i;
            highestScore = scores[i];
            winnerCount = 1;
        }
        else if(scores[i] == highestScore)
        {
            winners[winnerCount] = i;
            winnerCount++;
        }
        strfmt(buffer, "P%c: %d", (char)('1' + (i)), scores[i]);
        text(buffer, 6, 24+(i * 40));
    }

    if(winnerCount == 1)
    {
        strfmt(buffer, "P%d wins!", winners[0]+1);
        text(buffer, (SCREEN_SIZE / 2)-(4*8), 0);
    }
    else if(winnerCount == 2)
    {
        strfmt(buffer, "P%d and P%d tied!", winners[0]+1, winners[1]+1);
        text(buffer, (SCREEN_SIZE / 2)-(8*8)-4, 0);
    }
    else if(winnerCount == 3)
    {
        strfmt(buffer, "P%d, P%d, and P%d\ntied!", winners[0]+1, winners[1]+1, winners[2]+1);
        text(buffer, (SCREEN_SIZE / 2)-(11*8)-4, 0);
    }
    else 
    {
        strfmt(buffer, "Everybody tied!");
        text(buffer, (SCREEN_SIZE / 2)-(7*8)-4, 0);
    }

    text("Press \x80 to\ncontinue.", (SCREEN_SIZE / 2) - ((10*8)/2), SCREEN_SIZE-32);
}

static void name_entry_draw(GameState* state)
{
    *DRAW_COLORS = 0x3;
    char buffer[64];

    text("New record!", (SCREEN_SIZE/2)-((11*8)/2), 2);

    if(state->gameKind == GAME_KIND_NORMAL)
    {
        strfmt(buffer, "P%d's score: %3d", state->winningPlayer+1, state->winningScore);
        text(buffer, (SCREEN_SIZE/2)-((16*8)/2), 10);
    }
    else 
    {
        strfmt(buffer, "P1's streak: %3d", state->finalStreak);
        text(buffer, (SCREEN_SIZE/2)-((16*8)/2), 10);
    }

    constexpr i32 xstart = (20*8);
    strfmt(buffer, "Your name: %s%c", state->nameBuffer, ((state->cursorFrame >> 2) & 1)? '_': ' ');
    text(buffer, (SCREEN_SIZE/2)-(xstart/2), 24);

    hline(0, 36, SCREEN_SIZE);


    constexpr i32 xstart2 = (18*8);
    constexpr i32 ystart2 = 44;
    for(int i = 0; i < 5; i++)
    {
        const char* x = &sNameEntryCharMap[6*i];
        strfmt(buffer, " %c  %c  %c  %c  %c  %c ",
            x[0], x[1], x[2], x[3], x[4], x[5]);
        text(buffer, (SCREEN_SIZE/2)-(xstart2/2), ystart2+(i*9));
    }

    strcpy(buffer, "    BKSP  END ");
    text(buffer, (SCREEN_SIZE/2)-(xstart2/2), ystart2+(5*9));

    if(state->cursorOption >> 4 < 5)
    {
        i32 cursorX = ((SCREEN_SIZE/2)-(xstart2/2)) + 24*((state->cursorOption & 0xF));
        i32 cursorY = ystart2 + 9*(((state->cursorOption & 0xF0) >> 4));

        text(">", cursorX, cursorY);
    }
    else 
    {
        i32 cursorX = ((state->cursorOption & 0xF) < 3)
            ? ((SCREEN_SIZE/2)-(xstart2/2)) + 24
            : ((SCREEN_SIZE/2)-(xstart2/2)) + 48 + 24;
        i32 cursorY = ystart2 + 9*5;

        text(">", cursorX, cursorY);
    }
}

void streak_draw(const GameState* gs)
{
    *DRAW_COLORS = 0x3;
    char buffer[32];

    text("Survival Mode", (SCREEN_SIZE / 2)-((14*8)/2), 2);

    hline(0, 12, SCREEN_SIZE);

    strfmt(buffer, "Streak: %4d", gs->streak);
    text(buffer, (SCREEN_SIZE / 2)-((11*8)/2), 20);

    text("Press \x80 to\ncontinue.", (SCREEN_SIZE / 2) - ((10*8)/2), SCREEN_SIZE-32);
}

void survival_results_draw(const GameState* gs)
{
    *DRAW_COLORS = 0x3;
    char buffer[32];

    text("Survival Mode", (SCREEN_SIZE / 2)-((14*8)/2), 2);

    hline(0, 12, SCREEN_SIZE);

    text("Game over!", (SCREEN_SIZE / 2)-((10*8)/2), 20);
    strfmt(buffer, "Final streak: %3d", gs->finalStreak);
    text(buffer, (SCREEN_SIZE / 2)-((17*8)/2), 32);

    text("Press \x80 to\ncontinue.", (SCREEN_SIZE / 2) - ((10*8)/2), SCREEN_SIZE-32);
}

#define meterCircleSrc 0, 0
#define meterCircleSz  16, 16
#define meterLArrowSrc 0, 16
#define meterRArrowSrc 8, 16
#define meterArrowSz  8, 16

static void meters_draw(UNUSED GameState* state)
{
    // Power meter
#if 0
    for(int i = 0; i < 4; i++)
    {
        if(state->ballBody.data.power > ((5-i) * 16)) { 
            *DRAW_COLORS = 0x4;
        }
        else {
            *DRAW_COLORS = 0x2;
        }
        rect(SCREEN_SIZE-32, SCREEN_SIZE-80 + 16*i, meterCircleSz);
        *DRAW_COLORS = 0x4021;
        blitSub(meter, SCREEN_SIZE-32, SCREEN_SIZE-80 + 16*i,
            meterCircleSz, meterCircleSrc, meterWidth, meterFlags);
    }
#else 
    *DRAW_COLORS = 0x13;
    text("POW", SCREEN_SIZE-32, SCREEN_SIZE-132);
    *DRAW_COLORS = 0x2;
    rect(SCREEN_SIZE-28, SCREEN_SIZE-116, 16, 100 - state->ballBody.data.power);
    *DRAW_COLORS = 0x4;
    rect(SCREEN_SIZE-28, SCREEN_SIZE-116 + (100-state->ballBody.data.power), 16, state->ballBody.data.power);
#endif

    // Sway meter
    if(state->ballBody.data.sway < -1) { 
        *DRAW_COLORS = 0x4;
    } else if(state->ballBody.data.sway < 0) {
        *DRAW_COLORS = 0x3;
    } else {
        *DRAW_COLORS = 0x2;
    }
    rect(SCREEN_SIZE-36, SCREEN_SIZE-16, meterArrowSz);
    *DRAW_COLORS = 0x4021;
    blitSub(meter, SCREEN_SIZE-36, SCREEN_SIZE-16,
        meterArrowSz, meterLArrowSrc, meterWidth, meterFlags);


    if(state->ballBody.data.sway == 0) { 
        *DRAW_COLORS = 0x4;
    }
    else {
        *DRAW_COLORS = 0x2;
    }
    rect(SCREEN_SIZE-28, SCREEN_SIZE-16, meterCircleSz);
    *DRAW_COLORS = 0x4021;
    blitSub(meter, SCREEN_SIZE-28, SCREEN_SIZE-16,
        meterCircleSz, meterCircleSrc, meterWidth, meterFlags);

    if(state->ballBody.data.sway > 1) { 
        *DRAW_COLORS = 0x4;
    } else if(state->ballBody.data.sway > 0) {
        *DRAW_COLORS = 0x3;
    } else {
        *DRAW_COLORS = 0x2;
    }
    rect(SCREEN_SIZE-12, SCREEN_SIZE-16, meterArrowSz);
    *DRAW_COLORS = 0x4021;
    blitSub(meter, SCREEN_SIZE-12, SCREEN_SIZE-16,
        meterArrowSz, meterRArrowSrc, meterWidth, meterFlags);
}

void game_draw(UNUSED GameState* state)
{
    char buffer[64];
    switch(state->view)
    {
    case GAME_VIEW_ALLEY:
        bowling_alley_back_draw(state);
        person_anim(&state->objects[11], state->playerState);
        object_draw_ordered(state->objects, state->camPosX, state->camPosY);
        //meters_draw(state);
        if(!(state->objects[10].sprite.visible || state->playerState == 50))
        {
            meters_draw(state);
        }
        break;
    case GAME_VIEW_YOUREUP:
        *DRAW_COLORS = 0x13;
        strfmt(buffer, "P%d! You're up!\nPress \x80 to\ncontinue.", state->turn + 1);
        text(buffer, (SCREEN_SIZE / 2) - (7*8), (SCREEN_SIZE / 2)-12);
        break;
    case GAME_VIEW_SCORE:
        stat_screen_draw(state, state->viewFrame);
        break;
    case GAME_VIEW_RESULTS:
        results_draw(state);
        break;
    case GAME_VIEW_NAMEENTRY:
        name_entry_draw(state);
        break;
    case GAME_VIEW_SURV_DESC:
        *DRAW_COLORS = 0x13;
        text("In Survival Mode,\npick up as many\nspares in a row\nas you can.\nGood luck!", (SCREEN_SIZE/2) - ((17*8)/2), (SCREEN_SIZE / 2)-((5*8)/2));
        text("\nPress \x80 to\ncontinue.", (SCREEN_SIZE/2) - ((10*8)/2), SCREEN_SIZE-64);
        break;
    case GAME_VIEW_STREAK:
        streak_draw(state);
        break;
    case GAME_VIEW_SURV_RES:
        survival_results_draw(state);
        break;
    }
#if 0
    *DRAW_COLORS = 0x41;
    strfmt(buffer, "tn: %d", 
        state->throwNum);
    text(buffer, 0, 0);
#endif
}