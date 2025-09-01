# 1 "syscall.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 285 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include/language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "syscall.c" 2
# 1 "./syscall.h" 1



# 1 "./types.h" 1



# 1 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/stdint.h" 1 3



# 1 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/musl_xc8.h" 1 3
# 5 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/stdint.h" 2 3
# 26 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/stdint.h" 3
# 1 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/bits/alltypes.h" 1 3
# 133 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/bits/alltypes.h" 3
typedef unsigned __int24 uintptr_t;
# 148 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/bits/alltypes.h" 3
typedef __int24 intptr_t;
# 164 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/bits/alltypes.h" 3
typedef signed char int8_t;




typedef short int16_t;




typedef __int24 int24_t;




typedef long int32_t;





typedef long long int64_t;
# 194 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/bits/alltypes.h" 3
typedef long long intmax_t;





typedef unsigned char uint8_t;




typedef unsigned short uint16_t;




typedef __uint24 uint24_t;




typedef unsigned long uint32_t;





typedef unsigned long long uint64_t;
# 235 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/bits/alltypes.h" 3
typedef unsigned long long uintmax_t;
# 27 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/stdint.h" 2 3

typedef int8_t int_fast8_t;

typedef int64_t int_fast64_t;


typedef int8_t int_least8_t;
typedef int16_t int_least16_t;

typedef int24_t int_least24_t;
typedef int24_t int_fast24_t;

typedef int32_t int_least32_t;

typedef int64_t int_least64_t;


typedef uint8_t uint_fast8_t;

typedef uint64_t uint_fast64_t;


typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;

typedef uint24_t uint_least24_t;
typedef uint24_t uint_fast24_t;

typedef uint32_t uint_least32_t;

typedef uint64_t uint_least64_t;
# 148 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/stdint.h" 3
# 1 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/bits/stdint.h" 1 3
typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
# 149 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/stdint.h" 2 3
# 5 "./types.h" 2
# 1 "./os_config.h" 1
# 6 "./types.h" 2


typedef void (*f_ptr)(void);


typedef enum {READY = 0, RUNNING, WAITING} state_t;


typedef struct tcb {
    uint8_t task_id;
    f_ptr task_func;
    state_t task_state;
    uint8_t task_priority;

    uint8_t BSR_reg;
    uint8_t WORK_reg;
    uint8_t STATUS_reg;
    uint8_t STACK[32];
    uint8_t *task_sp;
} tcb_t;


typedef struct f_aptos {
    tcb_t readyQueue[5];
    uint8_t readyQueueSize;
    tcb_t *taskRunning;
} f_aptos_t;
# 5 "./syscall.h" 2

void os_create_task(uint8_t id, f_ptr task_f, uint8_t prior);
void os_delay(uint8_t time);
void os_yield();
void os_change_state(state_t new_state);
# 2 "syscall.c" 2

void os_create_task(uint8_t id, f_ptr task_f, uint8_t prior)
{
    tcb_t new_task;

    new_task.task_id = id;
    new_task.task_func = task_f;
    new_task.task_priority = prior;
    new_task.task_state = READY;

    new_task.BSR_reg = 0;
    new_task.STATUS_reg = 0;
    new_task.WORK_reg = 0;
    new_task.task_sp = &new_task.STACK[0];



}

void os_delay(uint8_t time)
{

}

void os_yield()
{

}

void os_change_state(state_t new_state)
{

}
