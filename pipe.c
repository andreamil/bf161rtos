#include "pipe.h"
#include "sync.h"
#include "mem.h"
#include <xc.h>

void create_pipe(pipe_t *p)
{
    p->pipe_pos_read    = 0;
    p->pipe_pos_write   = 0;
    p->pipe_capacity    = PIPE_MAX_SIZE;
    // Aloca buffer dinamicamente
    p->pipe_data        = (char*)SRAMalloc(p->pipe_capacity);
    // Inicializa os sem�foros de controle do pipe
    sem_init(&p->pipe_sem_read, 0);
    sem_init(&p->pipe_sem_write, p->pipe_capacity);
}

void destroy_pipe(pipe_t *p)
{
    if (p->pipe_data) {
        SRAMfree((unsigned char*)p->pipe_data);
        p->pipe_data = 0;
    }
    p->pipe_capacity = 0;
}

void read_pipe(pipe_t *p, char *buffer)
{
    di();
    
    // Testa o sem�foro de leitura
    sem_wait(&p->pipe_sem_read);
    
    *buffer = p->pipe_data[p->pipe_pos_read];
    p->pipe_pos_read = (p->pipe_pos_read + 1) % p->pipe_capacity;
    
    // Libera o sem�foro da escrita
    sem_post(&p->pipe_sem_write);
    
    ei();
}

void write_pipe(pipe_t *p, char buffer)
{
    di();
    
    // Testa o sem�foro de escrita
    sem_wait(&p->pipe_sem_write);
    
    p->pipe_data[p->pipe_pos_write] = buffer;
    p->pipe_pos_write = (p->pipe_pos_write + 1) % p->pipe_capacity;
    
    // Libera o sem�foro da leitura
    sem_post(&p->pipe_sem_read);
    
    ei();    
}

void pipe_write_block(pipe_t *p, const char *data, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        write_pipe(p, data[i]);
    }
}

void pipe_read_block(pipe_t *p, char *data, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        read_pipe(p, &data[i]);
    }
}

