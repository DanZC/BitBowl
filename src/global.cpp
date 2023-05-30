#include "global.hpp"

void state_set   (GlobalData* gdata, StateType type, void* data)
{
    gdata->type = type;
    state_init(gdata, data);
}

void state_init  (GlobalData* gdata, void* data)
{
    switch(gdata->type)
    {
    case STATE_TITLE:
        title_init(&gdata->title, data);
        break;
    case STATE_GAME:
        game_init(&gdata->game, data);
        break;
#if SOUND_TEST
    case STATE_SOUNDTEST:
        sound_test_init(&gdata->soundTest, data);
        break;
#endif
    }
}

void state_update(GlobalData* gdata)
{
    switch(gdata->type)
    {
    case STATE_TITLE:
        title_update(&gdata->title, gdata);
        break;
    case STATE_GAME:
        game_update(&gdata->game, gdata);
        break;
#if SOUND_TEST
    case STATE_SOUNDTEST:
        sound_test_update(&gdata->soundTest, gdata);
        break;
#endif
    }
}

void state_draw  (GlobalData* gdata)
{
    switch(gdata->type)
    {
    case STATE_TITLE:
        title_draw(&gdata->title);
        break;
    case STATE_GAME:
        game_draw(&gdata->game);
        break;
#if SOUND_TEST
    case STATE_SOUNDTEST:
        sound_test_draw(&gdata->soundTest);
        break;
#endif
    }
}