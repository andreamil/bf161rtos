#include "syscall.h"

void os_create_task(uint8_t id, f_ptr task_f, uint8_t prior)
{
    tcb_t new_task;
    
    new_task.task_id            = id;
    new_task.task_func          = task_f;
    new_task.task_priority      = prior;
    new_task.task_state         = READY;
    
    new_task.BSR_reg            = 0;
    new_task.STATUS_reg         = 0;
    new_task.WORK_reg           = 0;
    new_task.task_sp            = &new_task.STACK[0];
    
    // Inserir tarefa na fila de aptos
    
}

void os_delay(uint8_t time)
{
    
}

void os_yield()
{
    
}

void os_change_state(state_t new_state)
{
    
}

