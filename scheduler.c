#include "scheduler.h"
#include "os_config.h"
#include "syscall.h"
#include "kernel.h"

#include <stdio.h>

// Declara��o da fila de aptos
extern f_aptos_t readyQueue;


tcb_t *rr_scheduler()
{
    uint8_t pos_task_running = os_task_pos(readyQueue.taskRunning->task_func);
    uint8_t idle_selected = 0;
    
    do {
        pos_task_running = (pos_task_running+1) % readyQueue.readyQueueSize;
        if (readyQueue.readyQueue[pos_task_running].task_func == os_idle_task) {
            idle_selected++;
            if (idle_selected > 1) return &readyQueue.readyQueue[0];            
        }
        
    } while (readyQueue.readyQueue[pos_task_running].task_state != READY ||
             readyQueue.readyQueue[pos_task_running].task_func == os_idle_task);
    
    return &readyQueue.readyQueue[pos_task_running];
}

tcb_t *priority_scheduler()
{
    // Seleciona a tarefa READY com maior prioridade.
    // Critério de desempate: round-robin entre tarefas de mesma prioridade,
    // iniciando a busca a partir da posição da tarefa corrente.
    if (readyQueue.readyQueueSize == 0 || readyQueue.taskRunning == 0) {
        return &readyQueue.readyQueue[0]; // fallback para idle
    }

    uint8_t start = os_task_pos(readyQueue.taskRunning->task_func);
    uint8_t best_priority = 0;
    uint8_t i;

    // 1) Descobre a maior prioridade entre as tarefas prontas (exclui idle, a não ser que seja a única)
    uint8_t any_ready_non_idle = 0;
    for (i = 0; i < readyQueue.readyQueueSize; i++) {
        if (readyQueue.readyQueue[i].task_state == READY && readyQueue.readyQueue[i].task_func != os_idle_task) {
            any_ready_non_idle = 1;
            if (readyQueue.readyQueue[i].task_priority > best_priority) {
                best_priority = readyQueue.readyQueue[i].task_priority;
            }
        }
    }

    if (!any_ready_non_idle) {
        // apenas idle disponível
        return &readyQueue.readyQueue[0];
    }

    // 2) Entre as tarefas READY com 'best_priority', escolhe a próxima a partir de start+1
    uint8_t idx = start;
    uint8_t idle_selected = 0;
    do {
        idx = (idx + 1) % readyQueue.readyQueueSize;
        if (readyQueue.readyQueue[idx].task_state == READY &&
            readyQueue.readyQueue[idx].task_priority == best_priority &&
            readyQueue.readyQueue[idx].task_func != os_idle_task) {
            return &readyQueue.readyQueue[idx];
        }
        if (readyQueue.readyQueue[idx].task_func == os_idle_task) {
            idle_selected++;
            if (idle_selected > 1) return &readyQueue.readyQueue[0];
        }
    } while (1);
}

void scheduler()
{
#if DEFAULT_SCHEDULER == RR_SCHEDULER
    readyQueue.taskRunning = rr_scheduler();
#elif DEFAULT_SCHEDULER == PRIORITY_SCHEDULER
    readyQueue.taskRunning = priority_scheduler();
#endif    
}

