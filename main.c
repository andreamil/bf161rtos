#include <xc.h>
#include "syscall.h"
#include "kernel.h"


int main()
{
    os_config();
    
    // Cria tarefas de usuário
    
    os_start();
    
    while (1);
    
    return 0;
}
