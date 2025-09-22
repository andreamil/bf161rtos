#include <xc.h>

#include "user_app.h"
#include "syscall.h"
#include "sync.h"
#include "pipe.h"

//sem_t semaforo_teste;
pipe_t canal;

void config_app(void)
{
    TRISDbits.RD0 = TRISDbits.RD1 = TRISDbits.RD2 = 0;
    
    //sem_init(&semaforo_teste, 0);
    create_pipe(&canal);
    
    asm("GLOBAL _tarefa_1, _tarefa_2, _tarefa_3");
}

TASK tarefa_1(void)
{
    char dados[] = {'a', 'b', 'c'};
    int index = 0;
    while (1) {
        LATDbits.LD0 = ~PORTDbits.RD0;
        //sem_wait(&semaforo_teste);
        write_pipe(&canal, dados[index]);
        index = (index + 1) % 3;
    }
}

TASK tarefa_2(void)
{
    char dado;
    
    while (1) {
        LATDbits.LD1 = ~PORTDbits.RD1;
        //os_delay(200);
        read_pipe(&canal, &dado);
        if (dado == 'a') {
            
        }
        else if (dado == 'b') {
            
        }
        else if (dado == 'c') {
            
        }
    }    
}

TASK tarefa_3(void)
{
    while (1) {
        LATDbits.LD2 = ~PORTDbits.RD2;
        //sem_post(&semaforo_teste);
    }    
}

