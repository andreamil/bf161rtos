#include "sync.h"
#include "kernel.h"
#include "scheduler.h"
#include <xc.h>

// Declara��o da fila de aptos
extern f_aptos_t readyQueue;


void sem_init(sem_t *s, uint8_t init_value)
{
    s->contador         = init_value;
    s->sem_queue_in     = 0;
    s->sem_queue_out    = 0;    
}

void sem_wait(sem_t *s)
{
    di();
    
    // Decrementa contador
    s->contador--;
    
    if (s->contador < 0) {
        // Bloqueia a tarefa
        s->sem_queue[s->sem_queue_in] = readyQueue.taskRunning;
        s->sem_queue_in = (s->sem_queue_in + 1) % MAX_TASKS_ON_READY_QUEUE;
        // Troca de contexto
        SAVE_CONTEXT(WAITING_SEM);
        scheduler();
        RESTORE_CONTEXT();
    }
    
    ei();    
}

void sem_post(sem_t *s)
{
    di();
    
    s->contador++;
    
    if (s->contador <= 0) {
        s->sem_queue[s->sem_queue_out]->task_state = READY;
        s->sem_queue_out = (s->sem_queue_out + 1) % MAX_TASKS_ON_READY_QUEUE;
    }
    
    ei();
}

// ----------------------- Mutex implementation -----------------------
void mutex_init(mutex_t *m)
{
    m->locked = 0;
    m->owner = 0;
    m->q_in = 0;
    m->q_out = 0;
}

uint8_t mutex_trylock(mutex_t *m)
{
    uint8_t acquired = 0;
    di();
    if (m->locked == 0) {
        m->locked = 1;
        m->owner = readyQueue.taskRunning;
        acquired = 1;
    }
    ei();
    return acquired;
}

void mutex_lock(mutex_t *m)
{
    di();
    if (m->locked == 0) {
        // Adquire imediatamente
        m->locked = 1;
        m->owner = readyQueue.taskRunning;
        ei();
        return;
    }

    // Já está bloqueado: entra na fila e bloqueia a tarefa
    m->wait_queue[m->q_in] = readyQueue.taskRunning;
    m->q_in = (m->q_in + 1) % MAX_TASKS_ON_READY_QUEUE;
    SAVE_CONTEXT(WAITING_SEM);
    scheduler();
    RESTORE_CONTEXT();
    ei();
}

void mutex_unlock(mutex_t *m)
{
    di();
    // Apenas o dono libera; se não for o dono, ignora (alternativa: retornar erro)
    if (m->owner != readyQueue.taskRunning) {
        ei();
        return;
    }

    // Se houver tarefas esperando, acorda a próxima e transfere a posse
    if (m->q_in != m->q_out) {
        tcb_t *next = m->wait_queue[m->q_out];
        m->q_out = (m->q_out + 1) % MAX_TASKS_ON_READY_QUEUE;
        next->task_state = READY;
        m->owner = next;
        // Mantém locked = 1, pois o mutex segue adquirido pela próxima tarefa
    } else {
        // Ninguém esperando
        m->locked = 0;
        m->owner = 0;
    }
    ei();
}

