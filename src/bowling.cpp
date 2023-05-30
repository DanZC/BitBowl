#include "bowling.hpp"
#include "util.hpp"
#include "sound.hpp"
#include "save.hpp"
#include "random.hpp"

void do_ball_physics(BallBody* body, u32 frameCount)
{
    f32 dy = -((f32)body->data.power / 60.f);
    f32 dx = 0.f;

    // If the ball is skidding.
    if(body->data.skid > 0)
    {
        // If sway is very strong, sway immediately.
        if(abs(body->data.sway) > 4)
        {
            dx = ((f32)body->data.sway / 64.0f);

            if(body->data.power < 15 && (frameCount % 10 == 0))
            {
                // Sway more aggressively
                body->data.sway = body->data.sway + (i16)sgn(body->data.sway);
            }
        }

        // Reduce skid.
        body->data.skid--;
    }
    else
    {
        // Apply traction
        if(body->data.power > 10 && (frameCount % 10 == 0))
        {
            body->data.power--;
        }

        // Apply sway
        dx = ((f32)body->data.sway / 32.0f);

        if(body->data.power < 30 && (frameCount % 10 == 0))
        {
            
            if(body->data.sway == 0)
            {
                body->data.sway = (i16)random_int_range(0, 2) - 1;
            }
            else 
            {
                // Sway more aggressively
                body->data.sway = body->data.sway + (i16)sgn(body->data.sway);
            }
        }
    }
    body->dx = dx;
    body->dy = dy;
    body->x += dx;
    body->y += dy;
}

void handle_ball_collisions(BallBody* body, PinBody* pinBodies, u8 pinBodyCount, u8 optionFlags)
{
    // If the ball is in either gutter, don't bother with checking collisions.
    if(body->x <= 48.f)
    {
        body->x = 48.f;
        return;
    }

    if(body->x >= 104.f)
    {
        body->x = 104.f;
        return;
    }

    if(body->y <= 10.0f)
    {
        body->y = 10.0f;
        return;
    }

    // Check for pin collisions
    for(int i = 0; i < (int)pinBodyCount; i++)
    {
        PinBody* pBody = &pinBodies[i];

        // Skip pin collision if pin is already down
        if(pBody->pinState == PIN_DOWN) continue;

        // Check if ball bounding box overlaps pin bounding box
        if(body->x + 2.f < pBody->x + 14.f
        && body->x + 8.f > pBody->x + 2.f
        && body->y + 2.f < pBody->y + 16.f 
        && body->y + 8.f > pBody->y + 2.f)
        {
            if(pBody->pinState == PIN_STANDING || pBody->pinState == PIN_ROCKING)
            {
                // Using the balls speed and direction, apply force to the pin.
                pBody->pinState = PIN_FALLING | PIN_STATE_CHANGED;
                pBody->dy = body->dy / 4.f;

                if((body->x + 4.f) < (pBody->x + 8.f))
                {
                    //pBody->dx = cdist / 4.f;
                    pBody->dx = ((f32)body->data.power / 24.f);
                    //pBody->power = body->data.power / 2;
                    //pBody->dx = -body->dx / 2.f;
                }
                else 
                {
                    //pBody->dx = cdist / 4.f;
                    //pBody->dx = ((body->x + body->dx) - pBody->x) / 16.f;
                    //pBody->dx = -body->dx / 2.f;
                    pBody->dx = -((f32)body->data.power / 24.f);
                }
                body->x += -pBody->dx / 4.f;
                pBody->x += pBody->dx;

                if(optionFlags & OPTION_SOUND)
                {
                    u32 toneFlags = (pBody->x < 66.f)? TONE_PAN_LEFT: (pBody->x > 86.f)? TONE_PAN_RIGHT: 0;
                    sound_hit_pin(toneFlags);
                }
                
                //tone((240 << 16) | 150, (1 << 8) | 1, 80, toneFlags);
            
                // Slow down ball as it collides
                if(body->data.power > 10)
                {
                    body->data.power--;
                }
            }
            else if(pBody->pinState == PIN_FALLING)
            {
                //pBody->dx /= 1.5f;
                pBody->x += pBody->dx;
                //if(pBody->power > 0) pBody->power--;
            }
        }
    }
}

void handle_pin_collisions(PinBody* pinBodies, u8 pinBodyCount)
{
    for(int i = 0; i < (u8)pinBodyCount; i++)
    {
        auto pBody = &pinBodies[i];

        // If the pin is already still, don't check collisions.
        if(pBody->pinState == PIN_DOWN || pBody->pinState == PIN_STANDING) continue;

        //pBody->x += pBody->dx;
        pBody->y += pBody->dy;
        if(pBody->y < 10.f) pBody->y = 10.f;
        if(pBody->x < 48.f) pBody->x = 48.f;
        if(pBody->x > 104.f) pBody->x = 104.f;

        // If the pin is moving too slowly, don't check collisions.
        if((pBody->dx < 0.005f && pBody->dx > -0.005f)
        && (pBody->dy < 0.005f && pBody->dy > -0.005f)) continue;

        for(int j = i+1; j < (u8)pinBodyCount; j++)
        {
            // Pin cannot interact with itself.
            if(i == j) continue;

            auto pBody2 = &pinBodies[j];

            if(pBody2->pinState != PIN_STANDING && pBody2->pinState != PIN_ROCKING) continue;

            if(pBody->x + 2.f < pBody2->x + 15.f
            && pBody->x + 15.f > pBody2->x + 2.f
            && pBody->y + 0.f < pBody2->y + 16.f 
            && pBody->y + 16.f > pBody2->y + 0.f
            && pBody2->pinState == PIN_STANDING)
            {
                f32 dx = pBody->dx / 2.0f;
                f32 dy = pBody->dy / 2.0f;
                if(pBody->x > pBody2->x + 8.f && dx < -0.01f)
                {
                    pBody2->pinState = PIN_STATE_CHANGED | PIN_FALLING;
                    pBody2->dx = dx;
                    pBody2->dy = dy;
                }
                else if(pBody->x + 8.f < pBody2->x && dx > 0.01f)
                {
                    pBody2->pinState = PIN_STATE_CHANGED | PIN_FALLING;
                    pBody2->dx = dx;
                    pBody2->dy = dy;
                }
                else if(dx > 0.01f || dx < -0.01f)
                {
                    if(random_int_range(0, 1000) > 995)
                    {
                        pBody2->pinState = PIN_STATE_CHANGED | PIN_FALLING;
                        pBody2->dx = dx;
                        pBody2->dy = dy;
                    }
                    else 
                    {
                        pBody2->pinState = PIN_STATE_CHANGED | PIN_ROCKING;
                    }
                }
                else 
                {
                    pBody2->pinState = PIN_STATE_CHANGED | PIN_STANDING;
                }
            }
            else if(pBody->x < pBody2->x + 16.f
            && pBody->x + 16.f > pBody2->x
            && pBody->y + 0.f < pBody2->y + 16.f 
            && pBody->y + 16.f > pBody2->y + 0.f
            && pBody2->pinState == PIN_ROCKING)
            {
                f32 dx = pBody->dx / 2.0f;
                f32 dy = pBody->dy / 2.0f;
                if(dx > 0.01f || dx < -0.01f)
                {
                    if(random_int_range(0, 1000) > 995)
                    {
                        pBody2->pinState = PIN_STATE_CHANGED | PIN_FALLING;
                        pBody2->dx = dx;
                        pBody2->dy = dy;
                    }
                    else 
                    {
                        pBody2->dx = dx;
                        pBody2->dy = dy;
                        //pBody2->pinState = PIN_STATE_CHANGED | PIN_ROCKING;
                    }
                    //pBody2->pinState = PIN_STATE_CHANGED | PIN_FALLING;
                    //pBody2->dx = dx;
                    //pBody2->dy = dy;
                }
                else 
                {
                    pBody2->dx = dx;
                    pBody2->dy = dy;
                }
            }
        }
        pBody->dx /= 1.5f;
        pBody->dy /= 1.5f;
    }
}

u16 check_pins(PinBody* pinBodies)
{
    u16 result = 0;
    for(int i = 0; i < 10; i++)
    {
        result |= (pinBodies[i].pinState != PIN_DOWN) << i;
    }
    return result;
}

#define CHECK_PIN(N, X) (X & (1 << N))
#define CHECK_PIN_DOWN(N, X) ((X & (1 << N)) == 0)

/*
i32 score_next_throw(const PlayerData* pd, i32 frame, i32 throwNum)
{
    if(frame == 9 && throwNum >= 2)
    {
        return 0;
    }
    if(frame == 9 && throwNum < 2)
    {
        if(pd->frames[frame].throws[throwNum + 1] == 10)
        {
            return score_next_throw(pd, frame, throwNum + 1) + score_next_throw(pd, frame, throwNum + 2);
        }
        else if(pd->frames[frame].throws[throwNum + 1] == THROW_NULL)
        {
            return -1;
        }
        else if(pd->frames[frame].throws[throwNum] == 10)
        {
            return score_next_throw(pd, frame, throwNum + 1);
        }
        return pd->frames[frame].throws[throwNum];
    }
    if(throwNum >= 1)
    {

    }
}

i32 score_player(const PlayerData* pd, i32 frame)
{
    u8 throws[21];
    i32 tIdx = 0;
    for(i32 i = 0; i < frame; i++)
    {
        if(pd->frames[i].throws[0] == 10)
        {
            throws[tIdx++] = pd->frames[i].throws[0];
            if(i == 9)
            {
                throws[tIdx++] = pd->frames[i].throws[1];
                throws[tIdx++] = pd->frames[i].throws[2];
            }
        }
        else
        {
            throws[tIdx++] = pd->frames[i].throws[0] & 0x7F;
            throws[tIdx++] = pd->frames[i].throws[1] & 0x7F;
            if(i == 9 && throws[tIdx-1] + throws[tIdx-2] == 10)
            {
                throws[tIdx++] = pd->frames[i].throws[2];
            }
            
        }
    }

    i32 score;

    for(int i = 0; i < 21; i++)
    {
        if(throws[i] == 10)
        {
            score += 10 + ((i+1 >= 21)? 0 : throws[i + 1]) + ((i+2 >= 21)? 0 : throws[i + 1]);
        }
        else if(throws[i] == THROW_NULL)
        {
            score = -1;
            break;
        }
        else if(throws[i] + throws[i+1] == 10)
        {
            score += 10 + ((i+1 >= 21)? 0 : throws[i + 1]);
            i++;
        }

    }
}*/

i32 score_player(const PlayerData* pd, i32 frame)
{
    i32 score = 0;
    for(i32 i = 0; i < frame; i++)
    {
        const Frame& f = pd->frames[i];
        if(f.throws[0] == 10)
        {
            score += 10;
            if(f.index == 9 && f.throws[1] != THROW_NULL)
            {
                score += f.throws[1] & 0x7F;
                if(f.throws[2] != THROW_NULL)
                {
                    score += f.throws[2] & 0x7F;
                }
            }
            else if(pd->frames[f.index + 1].throws[0] == 10)
            {
                score += 10;
                const Frame& f2 = pd->frames[f.index + 1];
                if(f2.index == 9 && f2.throws[1] != THROW_NULL)
                {
                    score += f2.throws[1] & 0x7F;
                    /*if(f2.throws[2] != THROW_NULL)
                    {
                        score += f2.throws[2] & 0x7F;
                    }*/
                }
                /*else if(f2.index == 9 && f2.throws[1] != THROW_NULL)
                {
                    score += f2.throws[1] & 0x7F;
                    if(f2.throws[2] != THROW_NULL)
                    {
                        score += f2.throws[2] & 0x7F;
                    }
                }*/
                else if(pd->frames[f.index + 2].throws[0] == 10)
                {
                    score += pd->frames[f.index + 2].throws[0] & 0x7F;
                }
                else if(pd->frames[f.index + 2].throws[0] == THROW_NULL || pd->frames[f.index + 2].throws[1] == THROW_NULL)
                {
                    score = -1;
                }
                else 
                {
                    const Frame& f3 = pd->frames[f.index + 2];
                    score += (f3.throws[0] & 0x7F) + (f3.throws[1] & 0x7F);
                }
            }
            else if(pd->frames[f.index + 1].throws[0] == THROW_NULL || pd->frames[f.index + 1].throws[1] == THROW_NULL)
            {
                score = -1;
            }
            else 
            {
                score += (pd->frames[f.index + 1].throws[0] & 0x7f) 
                       + (pd->frames[f.index + 1].throws[1] & 0x7f);
            }
        }
        else if(f.throws[0] == THROW_NULL || f.throws[1] == THROW_NULL)
        {
            score = -1;
        }
        else if(f.throws[0] + f.throws[1] == 10)
        {
            score += 10;
            if(f.index == 9 && f.throws[2] != THROW_NULL)
            {
                score += f.throws[2] & 0x7f;
            }
            else if(pd->frames[f.index + 1].throws[0] == 10)
            {
                score += 10;
                /*const Frame& f2 = pd->frames[f.index + 1];
                if(f2.index == 9 && f2.throws[1] != THROW_NULL)
                {
                    score += f2.throws[1] & 0x7f;
                    //if(f2.throws[2] != THROW_NULL)
                    //{
                    //    score += f2.throws[2] & 0x7f;
                    //}
                }
                else if(f2.index == 9 && f2.throws[1] != THROW_NULL)
                {
                    score += f2.throws[1] & 0x7f;
                    //if(f2.throws[2] != THROW_NULL)
                    //{
                    //    score += f2.throws[2] & 0x7f;
                    //}
                }*/
            }
            else if(pd->frames[f.index + 1].throws[0] == THROW_NULL)
            {
                score = -1;
            }
            else 
            {
                score += pd->frames[f.index + 1].throws[0] & 0x7f;
            }
        }
        else 
        {
            score += (f.throws[0] & 0x7f) + (f.throws[1] & 0x7f);
        }

        if(score == -1)
        {
            break;
        }
    }
    return score;
}

void init_player_data(PlayerData* data)
{
    for(int i = 0; i < 10; i++)
    {
        data->frames[i].index = (u8)i;
        memset(data->frames[i].throws, sizeof(data->frames[i].throws), THROW_NULL);
    }
}