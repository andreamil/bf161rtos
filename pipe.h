#ifndef PIPE_H
#define	PIPE_H

#include "types.h"

void create_pipe(pipe_t *p);
void destroy_pipe(pipe_t *p);
void read_pipe(pipe_t *p, char *buffer);
void write_pipe(pipe_t *p, char buffer);

// Operações em bloco para enviar/receber estruturas maiores via pipe
void pipe_write_block(pipe_t *p, const char *data, uint8_t len);
void pipe_read_block(pipe_t *p, char *data, uint8_t len);

#endif	/* PIPE_H */

