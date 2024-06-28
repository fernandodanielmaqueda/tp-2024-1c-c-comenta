
#ifndef KERNEL_SCHEDULER_H
#define KERNEL_SCHEDULER_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "commons/log.h"
#include "commons/config.h"
#include "commons/string.h"
#include "commons/collections/list.h"
#include "commons/collections/dictionary.h"
#include "utils/module.h"
#include "utils/socket.h"
#include "kernel.h"

typedef enum e_Scheduling_Algorithm {
    FIFO_SCHEDULING_ALGORITHM,
    RR_SCHEDULING_ALGORITHM,
    VRR_SCHEDULING_ALGORITHM
} e_Scheduling_Algorithm;

typedef struct t_Scheduling_Algorithm {
    char *name;
    t_PCB *(*function_fetcher) (void);
} t_Scheduling_Algorithm;

extern e_Scheduling_Algorithm SCHEDULING_ALGORITHM;

extern const char *EXIT_REASONS[];

extern t_Quantum QUANTUM;
extern pthread_t THREAD_QUANTUM_INTERRUPT;
extern pthread_mutex_t MUTEX_QUANTUM_INTERRUPT;
extern int QUANTUM_INTERRUPT;

extern t_list *LIST_NEW;
extern t_list *LIST_READY;
extern t_list *LIST_READY_PRIORITARY;
extern t_list *LIST_EXECUTING;
extern t_list *LIST_BLOCKED;
extern t_list *LIST_EXIT;

extern pthread_mutex_t MUTEX_LIST_NEW;
extern pthread_mutex_t MUTEX_LIST_READY;
extern pthread_mutex_t MUTEX_LIST_READY_PRIORITARY;
extern pthread_mutex_t MUTEX_LIST_BLOCKED;
extern pthread_mutex_t MUTEX_LIST_EXECUTING;
extern pthread_mutex_t MUTEX_LIST_EXIT;

/*
extern sem_t sem_detener_execute;
extern sem_t sem_detener_new_ready;
extern sem_t sem_detener_block_ready;
extern sem_t sem_detener_block;
extern sem_t sem_detener_planificacion;
*/

extern pthread_t THREAD_LONG_TERM_SCHEDULER_NEW;
extern pthread_t THREAD_LONG_TERM_SCHEDULER_EXIT;
extern pthread_t THREAD_SHORT_TERM_SCHEDULER;

extern sem_t SEM_LONG_TERM_SCHEDULER_NEW;
extern sem_t SEM_LONG_TERM_SCHEDULER_EXIT;
extern sem_t SEM_SHORT_TERM_SCHEDULER;

extern unsigned int MULTIPROGRAMMING_LEVEL;
extern sem_t SEM_MULTIPROGRAMMING_LEVEL;
extern unsigned int MULTIPROGRAMMING_DIFFERENCE;
extern pthread_mutex_t MUTEX_MULTIPROGRAMMING_DIFFERENCE;
extern sem_t SEM_MULTIPROGRAMMING_POSTER;
extern pthread_t THREAD_MULTIPROGRAMMING_POSTER;

extern int LIST_PROCESS_STATES;
extern pthread_mutex_t MUTEX_LIST_PROCESS_STATES;
extern pthread_cond_t COND_LIST_PROCESS_STATES;
extern sem_t SEM_SWITCHING_STATES_COUNT;
extern pthread_cond_t COND_SWITCHING_STATES;

extern t_PID PID_COUNTER;

int find_scheduling_algorithm(char *name, e_Scheduling_Algorithm *destination);
void initialize_long_term_scheduler(void);
void initialize_short_term_scheduler(void);
void *long_term_scheduler_new(void*);
void *long_term_scheduler_exit(void *NULL_parameter);
void *short_term_scheduler(void*);
void *multiprogramming_poster(void *NULL_argument);
t_PCB *FIFO_scheduling_algorithm(void);
t_PCB *RR_scheduling_algorithm(void);
t_PCB *VRR_scheduling_algorithm(void);
void switch_process_state(t_PCB* pcb, e_Process_State NEW_STATE);
t_PCB *pcb_create();
void* start_quantum();
void stop_planificacion(void);
void init_planificacion(void);

#endif // KERNEL_SCHEDULER_H