#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int server_fd, new_socket, valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
char buffer[BUFFER_SIZE] = {0};

// Crear el socket
if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
}

// Configurar opciones del socket
if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
}

// Configurar dirección del servidor
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(PORT);

// Asignar dirección al socket
if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
}

// Escuchar conexiones entrantes
if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
}

while (1) {
    // Aceptar conexión entrante
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
    }

    // Leer datos enviados por el cliente
    valread = read(new_socket, buffer, BUFFER_SIZE);

    // Procesar solicitud del cliente
    if (strncmp(buffer, "init", 4) == 0) {
    // Llamar a la función init
    } else if (strncmp(buffer, "set", 3) == 0) {
    // Llamar a la función set_value
    } else if (strncmp(buffer, "get", 3) == 0) {
    // Llamar a la función get_value
    } else if (strncmp(buffer, "modify", 6) == 0) {
    // Llamar a la función modify_value
    } else if (strncmp(buffer, "delete", 6) == 0) {
    // Llamar a la función delete_key
    } else if (strncmp(buffer, "exist", 5) == 0) {
    // Llamar a la función exist
    } else if (strncmp(buffer, "copy", 4) == 0) {
    // Llamar a la función copy_key
    }

    // Limpiar buffer
    memset(buffer, 0, BUFFER_SIZE);

    // Cerrar conexión
    close(new_socket);
}
