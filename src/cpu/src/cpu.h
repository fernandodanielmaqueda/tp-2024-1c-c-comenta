/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include "commons/log.h"
#include "commons/config.h"
#include "commons/string.h"
#include "commons/memory.h"
#include "commons/bitarray.h"
#include "commons/collections/list.h"
#include "commons/collections/queue.h"
#include "utils/module.h"
#include "utils/arguments.h"
#include "utils/send.h"
#include "utils/socket.h"
#include "socket.h"
#include "opcodes.h"
#include "registers.h"

typedef enum e_TLB_Algorithm {
    FIFO_TLB_ALGORITHM,
    LRU_TLB_ALGORITHM
} e_TLB_Algorithm;

typedef struct t_TLB {
	t_PID PID;
	t_Page_Number page_number;
	t_Frame_Number frame;
    int time; //para el LRU
} t_TLB;

extern t_log *MODULE_LOGGER;
extern t_log *SOCKET_LOGGER;
extern t_config *MODULE_CONFIG;

extern t_PCB PCB;
extern pthread_mutex_t MUTEX_PCB;

extern int EXECUTING;
extern pthread_mutex_t MUTEX_EXECUTING;

extern e_Kernel_Interrupt KERNEL_INTERRUPT;
extern pthread_mutex_t MUTEX_KERNEL_INTERRUPT;

extern int SYSCALL_CALLED;
extern t_Payload *SYSCALL_INSTRUCTION;

extern int TLB_ENTRY_COUNT;

extern const char *TLB_ALGORITHMS[];

extern e_TLB_Algorithm TLB_ALGORITHM;

extern t_MemorySize PAGE_SIZE;
extern long TIMESTAMP;
extern t_list *TLB;          // tlb que voy a ir creando para darle valores que obtengo de la estructura de t_tlb

extern pthread_mutex_t MUTEX_TLB;

#define MAX_CPU_INSTRUCTION_ARGUMENTS 1 + 5

int module(int, char*[]);
void initialize_mutexes(void);
void finish_mutexes(void);
void initialize_semaphores(void);
void finish_semaphores(void);
void read_module_config(t_config *module_config);
int find_tlb_algorithm(char *name, e_TLB_Algorithm *destination);
void initialize_sockets(void);
void finish_sockets(void);
void *cpu_dispatch_start_server_for_kernel(void *server_parameter);
void *cpu_interrupt_start_server_for_kernel(void *server_parameter);
void instruction_cycle(void);
void *kernel_cpu_interrupt_handler(void *NULL_parameter);
t_list* mmu(t_PID pid, t_Logical_Address dir_logica, size_t bytes_contenido);
int check_tlb(t_PID process_id, t_Page_Number page_number, t_Frame_Number *destination);
void request_data_in_memory(t_Frame_Number nro_frame_required, t_PID pid, t_Page_Number nro_page, int direc, int register_origin, int register_destination);
void request_data_out_memory(t_Frame_Number nro_frame_required, t_PID pid, t_Page_Number nro_page, int direc, int register_origin, int register_destination);
void request_frame_memory(t_PID pid, t_Page_Number page);
void add_to_tlb(t_PID pid , t_Page_Number page, t_Frame_Number frame);
void replace_tlb_input(t_PID pid, t_Page_Number page, t_Frame_Number frame);
void delete_tlb_entry_by_pid_on_resizing(t_PID pid, int resize_number);
void delete_tlb_entry_by_pid_deleted(t_PID pid);
void cpu_fetch_next_instruction(char **line);
void ask_memory_page_size(void);
t_Page_Quantity seek_quantity_pages_required(t_Logical_Address dir_log, size_t bytes);
void attend_write(t_PID pid, t_list *list_physical_addresses, void *source, size_t bytes);
void attend_read(t_PID pid, t_list *list_physical_addresses, void *destination, size_t bytes);

#endif /* CPU_H */