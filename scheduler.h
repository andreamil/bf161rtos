/* 
 * File:   scheduler.h
 * Author: Anderson L. F. Perez
 *
 * Created on 28 de Agosto de 2025, 16:31
 */

#ifndef SCHEDULER_H
#define	SCHEDULER_H

#include "types.h"

tcb_t *rr_scheduler();
tcb_t *priority_scheduler();
tcb_t *scheduler();


#endif	/* SCHEDULER_H */

