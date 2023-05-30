#include "task.hpp"

#define MAX_TASKS 8

static Task sTasks[MAX_TASKS];
constexpr u16 sTasks_size = sizeof(sTasks);

u8 task_create(void(*init)(Task*, u8*), 
               bool(*update)(Task*, GlobalData*),
               u8* data)
{
    u8 id = 0;

    // Iterate through all valid ids and look for an id
    // that isn't being used.
    for(id = 0; id < MAX_TASKS; id++)
    {
        // An unused id is an id with the state TASK_EMPTY.
        if(sTasks[id].state == TASK_EMPTY)
        {
            // Setup the task with the data provided by the
            // arguments.
            sTasks[id].state = TASK_RUNNING;
            sTasks[id].init = init;
            sTasks[id].init(&sTasks[id], data);
            sTasks[id].update = update;

            // Return the id of the new task.
            return id;
        }
    }

    // If we can't find a valid id that is free to use,
    // return the invalid id constant.
    return TASKID_INVALID;
}

bool task_update(u8 id, GlobalData* gdata)
{
    // Only process if the task has a valid id and 
    // is still running.
    if(id < MAX_TASKS && sTasks[id].state == TASK_RUNNING)
    {
        // Update the task. The task is finished when the
        // update function returns true.
        bool is_finished = sTasks[id].update(&sTasks[id], gdata);
        if(is_finished)
        {
            // Set the task's state to finished.
            sTasks[id].state = TASK_FINISHED;
            return true;
        }
    }
    return false;
}

bool task_is_finished(u8 id)
{
    if(id < MAX_TASKS)
        return sTasks[id].state == TASK_FINISHED;
    else return true;
}

#include "util.hpp"

void task_destroy(u8 id)
{
    // If the id passed is valid, we "destroy" the task
    // by zeroing out its memory.
    if(id < MAX_TASKS)
        memset((u8*)&sTasks[id], sizeof(Task), 0);
}

void task_destroy_finished_tasks()
{
    u8 id = 0;
    for(id = 0; id < MAX_TASKS; id++)
    {
        if(sTasks[id].state == TASK_FINISHED)
        {
            memset((u8*)&sTasks[id], sizeof(Task), 0);
        }
    }
}

u8* task_get_data(u8 id)
{
    if(id < MAX_TASKS)
        return sTasks[id].data;
    return nullptr;
}

i16 task_get_output(u8 id)
{
    assert(id < MAX_TASKS, "Invalid task id passed to task_get_output.");
    if(id < MAX_TASKS)
        return sTasks[id].output;
    return -1;
}

u8 task_get_id_from_ptr(Task* task)
{
    return (u8)(task - &sTasks[0]);
}

#define TASK_DATA_PARAM(T, IDX) (*(T*)&task->data[IDX])

#define WAIT_FRAMES TASK_DATA_PARAM(u16, 0)

static void wait_impl_init(Task* task, u8* data)
{
    WAIT_FRAMES = *(u16*)data;
    if(WAIT_FRAMES == 0) WAIT_FRAMES = 1;
}

static bool wait_impl_update(Task* task, GlobalData* gdata)
{
    (void)gdata; // gdata is unused

    WAIT_FRAMES--;
    return WAIT_FRAMES == 0;
}

u8 task_wait_frames(u16 frames)
{
    return task_create(wait_impl_init, wait_impl_update, 
                       (u8*)&frames);
}

#include "input.hpp"

struct WaitInputTaskData
{
    u8 input;
    u8 controller;
};

#define WAIT_INPUT TASK_DATA_PARAM(u8, 0)
#define WAIT_CTRLR TASK_DATA_PARAM(u8, 1)

static void wait_input_impl_init(Task* task, u8* data)
{
    WAIT_INPUT = ((WaitInputTaskData*)data)->input;
    WAIT_CTRLR = ((WaitInputTaskData*)data)->controller;
}

static bool wait_input_impl_update(Task* task, GlobalData* gdata)
{
    (void)gdata; // gdata is unused

    return (GAMEPAD_JUST_PRESSED(WAIT_CTRLR, WAIT_INPUT)) != 0;
}

u8 task_wait_input(u8 input, u8 controller)
{
    WaitInputTaskData witd{input, controller};
    return task_create(wait_input_impl_init, 
                       wait_input_impl_update, 
                       (u8*)&witd);
}

// Example wait code

// struct WSt { u8 state; u8 taskId; };

// static bool whatever(Wst* s, GlobalData* gdata)
// {
//     // switch over the task's state
//     switch(s->state)
//     {
//     case 0:
//         // Call the function creating the task.
//         // Store the id returned.
//         s->taskId = wait(45);
//
//         // Move to the next state.
//         s->state++;
//         fallthrough;
//     case 1:
//         // Check the task and exit early if the
//         // task isn't done yet.
//         if(!task_update(s->taskId, gdata)) return false;
//
//         // Optionally copy data that the task produced.
//         u32 out_data = *(u32*)task_get_data(s->taskId);
//
//         // Destroy the task since we're done using it.
//         task_destroy(s->taskId);
//         
//         // We're done, so return true.
//         return true;
//     default:
//         return true;
//     }
// }

