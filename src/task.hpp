#pragma once

#include "common.h"
#include "tinyptr.hpp"

struct GlobalData;
struct Task;

using Task_init_t   = TinyPtr<void(Task*, u8*)>;
using Task_update_t = TinyPtr<bool(Task*, GlobalData*)>;

enum TaskState: u8
{
    TASK_EMPTY,
    TASK_RUNNING,
    TASK_FINISHED
};

/*
 * Specialized object which executes something over multiple 
 * frames.
 * Its state can be queried from the outside.
 * Used to share pieces of code between states and modules.
 */
struct Task
{
    u8 state;
    u8 data[9];
    i16 output;

    Task_init_t init;
    Task_update_t update;
};
constexpr u32 Task_size = sizeof(Task);

#define CHECK_TASK_STRUCT_FIT(T) static_assert(sizeof(T) <= sizeof(Task::data), #T " is too big to fit into Task::data");

#define TASKID_INVALID 255

/*
 * Creates a task with the init and update functions.
 * Also passes data into the init function.
 */
u8 task_create(void(*init)(Task*, u8*), 
               bool(*update)(Task*, GlobalData*),
               u8* data);
/*
 * Updates the state of a task with id.
 * Returns true, if the task finishes that update.
 * Returns false, otherwise.
 */
bool task_update(u8 id, GlobalData* gdata);

/*
 * Checks whether the specified task with id is finished.
 */
bool task_is_finished(u8 id);

/*
 * Destroys task with id.
 */
void task_destroy(u8 id);

/*
 * Destroys all tasks which have finished.
 */
void task_destroy_finished_tasks();

/*
 * Gets a pointer to the data array of a task.
 * Used for when the task may generate data which the handler
 * may care about.
 * Returns nullptr if the task id is invalid.
 */
u8* task_get_data(u8 id);

/*
 * Gets the value of the output of a task.
 * Used for when the task returns a value which the handler
 * may care about.
 * Returns -1 if the task id is invalid.
 */
i16 task_get_output(u8 id);

/*
 * Gets the id of a task from its pointer value.
 */
u8 task_get_id_from_ptr(Task* task);

/// Task implementations

/*
 * Creates a task which waits a specified number of frames.
 * Returns the task id of the task.
 */
u8 task_wait_frames(u16 frames);

/*
 * Creates a task which waits for the player to press an input.
 * Returns the task id of the task.
 */
u8 task_wait_input(u8 input, u8 controller = 0);
