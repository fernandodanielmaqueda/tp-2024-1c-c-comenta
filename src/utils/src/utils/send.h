/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_SEND_H
#define UTILS_SEND_H

#include "utils/package.h"
#include "utils/serialize/cpu_opcode.h"
#include "utils/serialize/eviction_reason.h"
#include "utils/serialize/exec_context.h"
#include "utils/serialize/io_type.h"
#include "utils/serialize/kernel_interrupt.h"
#include "utils/serialize/list.h"
#include "utils/serialize/memory.h"
#include "utils/serialize/port_type.h"
#include "utils/serialize/return_value.h"
#include "utils/serialize/subheader.h"
#include "utils/serialize/subpayload.h"
#include "utils/serialize/text.h"

// Handshake


int send_port_type(e_Port_Type port_type, int fd_socket);


int receive_port_type(e_Port_Type *port_type, int fd_socket);


// De uso general


int send_header(e_Header header, int fd_socket);


int receive_expected_header(e_Header header, int fd_socket);


int send_text_with_header(e_Header header, char *text, int fd_socket);


int receive_text_with_expected_header(e_Header header, char **text, int fd_socket);


int send_return_value_with_header(e_Header header, t_Return_Value return_value, int fd_socket);


int receive_return_value_with_expected_header(e_Header expected_header, t_Return_Value *return_value, int fd_socket);


// Kernel - Memoria


int send_process_create(t_PID pid, char *instructions_path, t_Return_Value flag_relative_path, int fd_socket);


int send_process_destroy(t_PID pid, int fd_socket);


// Kernel - CPU


int send_process_dispatch(t_Exec_Context exec_context, int fd_socket); //INT HEADER TERCER PARAMETREO ENVIO PCB A CUALQUIEWR SOCKET


int receive_process_dispatch(t_Exec_Context *exec_context, int fd_socket);


int send_process_eviction(t_Exec_Context exec_context, e_Eviction_Reason eviction_reason, t_Payload syscall_instruction, int fd_socket);


int receive_process_eviction(t_Exec_Context *exec_context, e_Eviction_Reason *eviction_reason, t_Payload *syscall_instruction, int fd_socket);


int send_kernel_interrupt(e_Kernel_Interrupt type, t_PID pid, int fd_socket);


int receive_kernel_interrupt(e_Kernel_Interrupt *kernel_interrupt, t_PID *pid, int fd_socket);

// Kernel - Entrada/Salida

int send_interface_data(char *interface_name, e_IO_Type io_type, int fd_socket);


int receive_interface_data(char **interface_name, e_IO_Type *io_type, int fd_socket);


int send_io_operation_dispatch(t_PID pid, t_Payload io_operation, int fd_socket);


int receive_io_operation_dispatch(t_PID *pid, t_Payload *io_operation, int fd_socket);


int send_io_operation_finished(t_PID pid, t_Return_Value return_value, int fd_socket);


int receive_io_operation_finished(t_PID *pid, t_Return_Value *return_value, int fd_socket);


// CPU - Memoria

int send_instruction_request(t_PID pid, t_PC pc, int fd_socket);

#endif // UTILS_SEND_H