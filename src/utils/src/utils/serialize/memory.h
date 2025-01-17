/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_SERIALIZE_MEMORY_H
#define UTILS_SERIALIZE_MEMORY_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include "commons/config.h"
#include "commons/string.h"
#include "utils/package.h"

#include "commons/log.h"
#include "utils/module.h"

typedef uint32_t t_Size;

void size_serialize_element(t_Payload *payload, void *source);


void size_deserialize_element(t_Payload *payload, void **destination);


/**
 * @brief Serializacion de un size_t para ser enviado.
 * @param payload Payload a encolar.
 * @param source size_t fuente a serializar
 */
void size_serialize(t_Payload *payload, size_t source);


/**
 * @brief Deserializacion de un size_t para ser leido.
 * @param payload Payload a desencolar.
 * @param destination Destino del size_t deserializado
 */
void size_deserialize(t_Payload *payload, size_t *destination);


/**
 * @brief Loguea un size_t.
 * @param source size_t a loguear.
 */
void size_log(size_t source);

#endif // UTILS_SERIALIZE_MEMORY_H