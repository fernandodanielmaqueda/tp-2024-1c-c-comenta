/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "cpu.h"

char *MODULE_NAME = "cpu";
char *MODULE_LOG_PATHNAME = "cpu.log";
char *MODULE_CONFIG_PATHNAME = "cpu.config";

t_log *MODULE_LOGGER;
t_config *MODULE_CONFIG;

// Tipos de interrupciones para el ciclo
int interruption_io;

int CANTIDAD_ENTRADAS_TLB;
char *ALGORITMO_TLB;

int size_pag;
long timestamp;
int direccion_logica; // momentaneo hasta ver de donde la saco
t_list *tlb;          // tlb que voy a ir creando para darle valores que obtengo de la estructura de t_tlb

// Variables para trabajar con las instrucciones
int nro_page = 0;
uint32_t value = 0;

t_PCB *PCB;
// char *recurso = NULL;
// no sirveaca me aprece--> t_PCB new_pcb;
e_Register register_origin;
e_Register register_destination;

e_Interrupt INTERRUPT;

int dir_logica_origin = 0;
int dir_logica_destination = 0;

int dir_fisica_origin = 0;
int dir_fisica_destination = 0;

uint32_t unit_work = 0;
char *interfaz = NULL;

pthread_mutex_t sem_mutex_tlb;

const char *t_register_string[] = {
    [AX_REGISTER] = "AX",
    [BX_REGISTER] = "BX",
    [CX_REGISTER] = "CX",
    [DX_REGISTER] = "DX",
    [EAX_REGISTER] = "EAX",
    [EBX_REGISTER] = "EBX",
    [ECX_REGISTER] = "ECX",
    [EDX_REGISTER] = "EDX",
    [RAX_REGISTER] = "RAX",
    [RBX_REGISTER] = "RBX",
    [RCX_REGISTER] = "RCX",
    [RDX_REGISTER] = "RDX",
    [SI_REGISTER] = "SI",
    [DI_REGISTER] = "DI"};

const char *t_interrupt_type_string[] = {
    [ERROR_CAUSE] = "ERROR_CAUSE",
    [SYSCALL_CAUSE] = "SYSCALL_CAUSE",
    [INTERRUPTION_CAUSE] = "INTERRUPTION_CAUSE"};

int module(int argc, char *argv[])
{

    initialize_loggers();
    initialize_configs();
    initialize_sockets();

    log_debug(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);

    instruction_cycle();

    // finish_threads();
    finish_sockets();
    // finish_configs();
    finish_loggers();

    return EXIT_SUCCESS;
}

void read_module_config(t_config *MODULE_CONFIG)
{
    CONNECTION_MEMORY = (t_Connection){.client_type = CPU_TYPE, .server_type = MEMORY_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_MEMORIA"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_MEMORIA")};
    SERVER_CPU_DISPATCH = (t_Single_Client_Server){.server = (t_Server){.server_type = CPU_DISPATCH_TYPE, .clients_type = KERNEL_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA_DISPATCH")}};
    SERVER_CPU_INTERRUPT = (t_Single_Client_Server){.server = (t_Server){.server_type = CPU_INTERRUPT_TYPE, .clients_type = KERNEL_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA_INTERRUPT")}};
    CANTIDAD_ENTRADAS_TLB = config_get_int_value(MODULE_CONFIG, "CANTIDAD_ENTRADAS_TLB");
    ALGORITMO_TLB = config_get_string_value(MODULE_CONFIG, "ALGORITMO_TLB");
}

void instruction_cycle(void)
{
    t_Arguments *IR;
    t_CPU_OpCode *opcode;

    tlb = list_create();

    while (1)
    {

        PCB = cpu_receive_pcb();
        log_trace(MODULE_LOGGER, "PCB recibido del proceso : %i - Ciclo de instruccion ejecutando", PCB->PID);

        while(1)
        {

            // FETCH

            // FALTA ENVIAR A MEMORIA UNA SOLICITUD DE INSTRUCCION (PC)
            // cpu_request_next_instruction();
            IR = cpu_receive_cpu_instruction();

            // DECODE
            opcode = decode_instruction(IR->argv[0]);
            if (opcode == NULL)
            {
                log_error(MODULE_LOGGER, "%s: Error al decodificar la instruccion", IR->argv[0]);
                exit(EXIT_FAILURE);
            }

            // EXECUTE
            opcode->function(IR->argc, IR->argv);

            // CHECK INTERRUPT
            

            //INTERRUPT = cpu_receive_interrupt_type();
            if(INTERRUPT == SYSCALL_CAUSE)
                break; // DESALOJO
        }

        // CHEQUEAR EL TIPO DE INTERRUPCION

        /*
        if (interrupt != SYSCALL_CAUSE)
        {

            if ()
        }
        */

        // TODO :: MANDO PCB CON LA INFO DEL FIN DE PROCESO

        t_Package *package = package_create_with_header(SUBHEADER_HEADER);
        pcb_serialize(package->payload, PCB);
        interrupt_serialize(package->payload, &INTERRUPT);
        arguments_serialize(package->payload, IR);
        // package_send
    }
}

void decode_execute(t_CPU_Instruction *instruction, t_PCB *pcb)
{
    // size_t largo_nombre = 0;

    // int size_pag = request_sizePag_memory(); // TODO: DESARROLLAR EN MEMORIA
    // inncesario aca me parece---->int nro_frame_required = request_frame_memory(nro_page, pcb->PID);

    // TODO:::: MANDAR POR PCB EL TIPO D EINTERRUPCION DEL PROCESO
}

int string_to_register(const char *string)
{

    if (strcmp(string, "AX") == 0)
        return AX_REGISTER;
    else if (strcmp(string, "BX") == 0)
        return BX_REGISTER;
    else if (strcmp(string, "CX") == 0)
        return CX_REGISTER;
    else if (strcmp(string, "DX") == 0)
        return DX_REGISTER;
    else if (strcmp(string, "EAX") == 0)
        return EAX_REGISTER;
    else if (strcmp(string, "EBX") == 0)
        return EBX_REGISTER;
    else if (strcmp(string, "ECX") == 0)
        return ECX_REGISTER;
    else if (strcmp(string, "EDX") == 0)
        return EDX_REGISTER;
    else if (strcmp(string, "RAX") == 0)
        return RAX_REGISTER;
    else if (strcmp(string, "RBX") == 0)
        return RBX_REGISTER;
    else if (strcmp(string, "RCX") == 0)
        return RCX_REGISTER;
    else if (strcmp(string, "SI") == 0)
        return SI_REGISTER;
    else if (strcmp(string, "DI") == 0)
        return DI_REGISTER;

    else
    {
        log_error(MODULE_LOGGER, "Se intentó convertir string a registro un parámetro que no es registro.");
        exit(EXIT_FAILURE);
    }
}

int mmu(uint32_t dir_logica, t_PCB *pcb, int tamanio_pagina, int register_otrigin, int register_destination, int in_out)
{

    int nro_page = floor(dir_logica / tamanio_pagina);
    int offset = dir_logica - nro_page * tamanio_pagina;
    int nro_frame_required = 0;
    int dir_fisica = 0;

    // CHEQUEO SI ESTA EN TLB EL FRAME QUE NECESITO
    pthread_mutex_lock(&sem_mutex_tlb); // DUDA CONE ESTO!!
    int frame_tlb = check_tlb(pcb->PID, nro_page);
    pthread_mutex_unlock(&sem_mutex_tlb); // DUDA CONE ESTO!!

    if (frame_tlb != -1)
    {
        nro_frame_required = frame_tlb;
        log_info(MODULE_LOGGER, "PID: %i - TLB HIT - PAGINA: %i ", pcb->PID, nro_page);
        tlb_access(pcb, nro_page, nro_frame_required, dir_logica, register_otrigin, register_destination, in_out);
    }
    else
    {
        request_frame_memory(nro_page, pcb->PID);
        // FALTA nro_frame_required RECV
        log_info(MODULE_LOGGER, "PID: %i - TLB MISS - PAGINA: %i", pcb->PID, nro_page);
    }

    dir_fisica = nro_frame_required * tamanio_pagina + offset;
    return dir_fisica;
}

int check_tlb(int process_id, int page_number)
{

    t_TLB *tlb_entry = NULL;
    int frame_number = -1;
    for (int i = 0; i < list_size(tlb); i++)
    {

        tlb_entry = list_get(tlb, i);
        if (tlb_entry->PID == process_id && tlb_entry->page_number == page_number)
        {
            frame_number = tlb_entry->frame;

            // ==============TODO:FALTA ALGORITMO FIFO ===============//

            if (strcmp(ALGORITMO_TLB, "LRU") == 0)
            {
                tlb_entry->time = timestamp;
                timestamp++;
            }
        }
    }
    return frame_number;
}

void tlb_access(t_PCB *pcb, int nro_page, int frame_number_required, int direc, int register_origin, int register_destination, int in_out)
{

    if (in_out == IN)
    {

        // TODO : CON BRIAN PEDIR A MEMORIA QUE HAGA ESTAS FUNCIONES..
        request_data_in_memory(frame_number_required, pcb->PID, nro_page, direc, register_origin, register_destination);
        log_info(MODULE_LOGGER, "PID: %i -Accion:LEER - Pagina: %i - Direccion Fisica: %i %i ", pcb->PID, nro_page, frame_number_required, direc);
    }
    else
    {
        request_data_out_memory(frame_number_required, pcb->PID, nro_page, direc, register_origin, register_destination);
        log_info(MODULE_LOGGER, "PID: %i -Accion:ESCRIBIR - Pagina: %i - Direccion Fisica: %i %i ", pcb->PID, nro_page, frame_number_required, direc);
    }
}

void request_data_in_memory(int frame_number_required, int pid, int nro_page, int direc, int register_origin, int register_destination)
{
}

void request_data_out_memory(int frame_number_required, int pid, int nro_page, int direc, int register_origin, int register_destination)
{
}

t_PCB *cpu_receive_pcb(void)
{
    t_PCB *pcb;

    t_Package *package = package_receive(SERVER_CPU_DISPATCH.client.fd_client);
    switch (package->header)
    {
    case PCB_HEADER:
        pcb = pcb_deserialize(package->payload);
        break;
    default:
        log_error(SERIALIZE_LOGGER, "HeaderCode pcb %d desconocido", package->header);
        exit(EXIT_FAILURE);
        break;
    }
    package_destroy(package);

    return pcb;
}

t_Arguments *cpu_receive_cpu_instruction(void)
{
    t_Arguments *instruction;

    t_Package *package = package_receive(CONNECTION_MEMORY.fd_connection);
    switch (package->header)
    {
    case CPU_INSTRUCTION_HEADER:
        instruction = arguments_deserialize(package->payload);
        break;
    default:
        log_error(SERIALIZE_LOGGER, "Header %d desconocido", package->header);
        exit(EXIT_FAILURE);
        break;
    }
    package_destroy(package);

    return instruction;
}

e_Interrupt *cpu_receive_interrupt_type(void)
{

    e_Interrupt *interrupt;

    t_Package *package = package_receive(SERVER_CPU_INTERRUPT.client.fd_client);
    switch (package->header)
    {
    case INTERRUPT_HEADER:
        interrupt = interrupt_deserialize(package->payload);
        break;
    default:
        log_error(SERIALIZE_LOGGER, "Header interrupt %d desconocido", package->header);
        exit(EXIT_FAILURE);
        break;
    }
    package_destroy(package);

    return interrupt;
}

void request_frame_memory(int page, int pid) {
    t_Package *package = package_create_with_header(FRAME_REQUEST);
    payload_enqueue(package->payload, &page, sizeof(int));
    payload_enqueue(package->payload, &pid, sizeof(int));
    package_send(package, CONNECTION_MEMORY.fd_connection);
}
