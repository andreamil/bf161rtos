#include "scheduler.h"
#include "os_config.h"
#include "syscall.h"
#include "kernel.h"

#include <stdio.h>

// Declaração da fila de aptos
extern f_aptos_t readyQueue;


tcb_t *rr_scheduler()
{
    uint8_t pos_task_running = os_task_pos(readyQueue.taskRunning->task_func);
    
    // Da tarefa que está em execução para o fim
    for (uint8_t i = pos_task_running+i; i < readyQueue.readyQueueSize; i++) {
        if (readyQueue.readyQueue[i].task_state == READY) return &readyQueue.readyQueue[i];
    }

    // Da tarefa que está em execução para o fim
    for (uint8_t i = 1; i < pos_task_running; i++) {
        if (readyQueue.readyQueue[i].task_state == READY) return &readyQueue.readyQueue[i];
    }
    
    // Retorna IDLE
    return &readyQueue.readyQueue[0];    
}

tcb_t *priority_scheduler()
{
    tcb_t *next = NULL;
    
    return next;
}

tcb_t *scheduler()
{
#if DEFAULT_SCHEDULER == RR_SCHEDULER
    return rr_scheduler();
#elif DEFAULT_SCHEDULER == PRIORITY_SCHEDULER
    return priority_scheduler();
#endif    
}

