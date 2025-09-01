#include "scheduler.h"
#include "os_config.h"
#include "syscall.h"

// Declaração da fila de aptos
extern f_aptos_t readyQueue;


tcb_t *rr_scheduler()
{    
    // Versão Tabajar (melhorar)
    for (tcb_t *next = readyQueue.taskRunning; 
         next != &readyQueue.readyQueue[readyQueue.readyQueueSize];) {
        next++;
        if (next->task_state == READY) return next;
    }
    for (tcb_t *next = &readyQueue.readyQueue[0]; 
         next != &readyQueue.readyQueue[readyQueue.readyQueueSize];) {
        next++;
        if (next->task_state == READY) return next;
    }
    
    // Idle
    return &readyQueue.readyQueue[0];    
}

tcb_t *priority_scheduler()
{
    
}

tcb_t *scheduler()
{
#if DEFAULT_SCHEDULER == RR_SCHEDULER
    return rr_scheduler();
#elif DEFAULT_SCHEDULER == PRIORITY_SCHEDULER
    return priority_scheduler();
#endif    
}

