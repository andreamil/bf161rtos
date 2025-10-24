#include <xc.h>
#include "syscall.h"
#include "kernel.h"
#include "user_app.h"

// Config Bits para usar oscilador interno e desabilitar recursos não usados
#pragma config FOSC = INTOSCIO_EC   // Oscilador interno, RA6/RA7 como I/O/EC
#pragma config WDT  = OFF           // Watchdog Timer desabilitado
#pragma config LVP  = OFF           // Programação em baixa tensão desabilitada
#pragma config PBADEN = OFF         // PORTB<4:0> digitais no reset
#pragma config MCLRE = ON           // MCLR habilitado
#pragma config FCMEN = OFF          // Fail-Safe Clock Monitor desabilitado
#pragma config IESO  = OFF          // Internal/External Switchover desabilitado
#pragma config BOR   = OFF          // Brown-out Reset desabilitado
#pragma config PWRT  = ON           // Power-up Timer habilitado


int main()
{
    os_config();

    // Prioridades: central (alta), motors (alta), sensores (média), bateria (média/baixa)
    // Maior número = maior prioridade
    os_create_task(200, task_central, 10);
    os_create_task(201, task_motors,   9);
    os_create_task(202, task_sensors,  6);
    os_create_task(203, task_battery,  5);    
    
    os_start();
    
    while (1);
    
    return 0;
}


