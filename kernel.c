#include "types.h"
#include "syscall.h"
#include "kernel.h"
#include "hardware.h"
#include "user_app.h"
#include <xc.h>

// Declaração da fila de aptos
f_aptos_t readyQueue;

void os_config(void)
{
    readyQueue.readyQueueSize   = 0;
    readyQueue.taskRunning      = &readyQueue.readyQueue[0];
    
    // Criar a tarefa idle
    os_create_task(0, os_idle_task, 1);
    
    asm("GLOBAL _os_idle_task");
}

void os_start(void)
{
    // Configurar o timer
    conf_interrupts();
    conf_timer_0();
    
    // Configurações de usuário
    config_app();
    
    // Habilita interrupções globais
    ei();
}


void os_idle_task(void)
{
    TRISDbits.RD3 = 0;
    
    while (1) {
        // Nop();
        LATDbits.LD3 = ~PORTDbits.RD3;
    }
}
