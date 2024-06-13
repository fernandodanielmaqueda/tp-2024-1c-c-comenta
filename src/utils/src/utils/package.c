/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "package.h"

t_Package *package_create(void) {

  t_Package *package = malloc(sizeof(t_Package));
  if(package == NULL) {
    log_error(SERIALIZE_LOGGER, "No se pudo crear el package con malloc");
    exit(EXIT_FAILURE);
  }

  package->payload = payload_create();

  return package;
}

t_Package *package_create_with_header(e_Header header) {
  t_Package *package = package_create();
  package->header = header;
  return package;
}

void package_destroy(t_Package *package) {
  if (package == NULL)
    return;
  payload_destroy(package->payload);
  free(package);
}

void package_send(t_Package *package, int fd_socket) {
  
  // Si el paquete es NULL, no se envia nada
  if(package == NULL)
    return;

  ssize_t bytes;

  size_t bufferSize = sizeof(t_Header_Serialized) + sizeof(package->payload->size) + (size_t) package->payload->size;

  void *buffer = malloc(bufferSize);
  if(buffer == NULL) {
    log_error(SERIALIZE_LOGGER, "No se pudo crear el buffer con malloc");
    exit(EXIT_FAILURE);
  }

  size_t offset = 0;

  offset = memcpy_serialize(buffer, offset, &(package->header), sizeof(t_Header_Serialized));
  offset = memcpy_serialize(buffer, offset, &(package->payload->size), sizeof(package->payload->size));
  offset = memcpy_serialize(buffer, offset, package->payload->stream, (size_t) package->payload->size);

  bytes = send(fd_socket, buffer, bufferSize, 0);

  if (bytes == -1) {
      log_error(SERIALIZE_LOGGER, "Funcion send: %s\n", strerror(errno));
      close(fd_socket);
      exit(EXIT_FAILURE);
  }
  if (bytes != bufferSize) {
      log_error(SERIALIZE_LOGGER, "Funcion send: No coinciden los bytes enviados (%zd) con los que se esperaban enviar (%zd)\n", bufferSize, bytes);
      close(fd_socket);
      exit(EXIT_FAILURE);
  }

  free(buffer);
}

t_Package *package_receive(int fd_socket) {
  t_Package *package = package_create();
  package_receive_header(package, fd_socket);
  package_receive_payload(package, fd_socket);
  return package;
}

void package_receive_header(t_Package *package, int fd_socket) {

  if(package == NULL)
    return;

  ssize_t bytes;

  t_Header_Serialized header_serialized;
  bytes = recv(fd_socket, (void *) &(header_serialized), sizeof(t_Header_Serialized), 0); // MSG_WAITALL

  if (bytes == 0) {
      log_error(SERIALIZE_LOGGER, "Desconectado [Emisor]\n");
      close(fd_socket);
      exit(EXIT_FAILURE);
  }
  if (bytes == -1) {
      log_error(SERIALIZE_LOGGER, "Funcion recv: %s\n", strerror(errno));
      close(fd_socket);
      exit(EXIT_FAILURE);
  }
  if (bytes != sizeof(t_Header_Serialized)) {
      log_error(SERIALIZE_LOGGER, "Funcion recv: No coinciden los bytes recibidos (%zd) con los que se esperaban recibir (%zd)\n", sizeof(t_Header_Serialized), bytes);
      close(fd_socket);
      exit(EXIT_FAILURE);
  }

  package->header = (e_Header) header_serialized;
}

void package_receive_payload(t_Package *package, int fd_socket) {

  if(package == NULL)
    return;

  ssize_t bytes;

  bytes = recv(fd_socket, &(package->payload->size), sizeof(t_PayloadSize), 0); // MSG_WAITALL

  if (bytes == 0) {
      log_error(SERIALIZE_LOGGER, "Desconectado [Emisor]\n");
      close(fd_socket);
      exit(EXIT_FAILURE);
  }
  if (bytes == -1) {
      log_error(SERIALIZE_LOGGER, "Funcion recv: %s\n", strerror(errno));
      close(fd_socket);
      exit(EXIT_FAILURE);
  }
  if (bytes != sizeof(t_PayloadSize)) {
      log_error(SERIALIZE_LOGGER, "Funcion recv: No coinciden los bytes recibidos (%zd) con los que se esperaban recibir (%zd)\n", sizeof(t_PayloadSize), bytes);
      close(fd_socket);
      exit(EXIT_FAILURE);
  }

  if(package->payload->size == 0)
    return;

  package->payload->stream = malloc(package->payload->size);
  if(package->payload->stream == NULL) {
    log_error(SERIALIZE_LOGGER, "No se pudo crear el stream con malloc");
    exit(EXIT_FAILURE);
  }

  bytes = recv(fd_socket, package->payload->stream, (size_t) package->payload->size, 0); // MSG_WAITALL

  if (bytes == 0) {
      log_error(SERIALIZE_LOGGER, "Desconectado [Emisor]\n");
      close(fd_socket);
      exit(EXIT_FAILURE);
  }
  if (bytes == -1) {
      log_error(SERIALIZE_LOGGER, "Funcion recv: %s\n", strerror(errno));
      close(fd_socket);
      exit(EXIT_FAILURE);
  }
  if (bytes != (size_t) package->payload->size) {
      log_error(SERIALIZE_LOGGER, "Funcion recv: No coinciden los bytes recibidos (%zd) con los que se esperaban recibir (%zd)\n", (size_t) package->payload->size, bytes);
      close(fd_socket);
      exit(EXIT_FAILURE);
  }
}