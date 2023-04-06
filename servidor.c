#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include "llist.h"???

#define PORT 8080

void procesar_peticion(int socket_cliente) {
    int op, clave, value2, result;
    double value3;
    char value1[255];
    struct tupla tupla;

    read(socket_cliente, &op, sizeof(int));
    switch (op) {
        case 0:
            result = init();
            write(socket_cliente, &result, sizeof(int));
            break;
        case 1:
            read(socket_cliente, &clave, sizeof(int));
            read(socket_cliente, value1, 255);
            read(socket_cliente, &value2, sizeof(int));
            read(socket_cliente, &value3, sizeof(double));
            result = set_value(clave, value1, value2, value3);
            write(socket_cliente, &result, sizeof(int));
            break;
        case 2:
            read(socket_cliente, &clave, sizeof(int));
            read(socket_cliente, value1, 255);
            read(socket_cliente, &value2, sizeof(int));
            read(socket_cliente, &value3, sizeof(double));
            result = get_value(clave, value1, &value2, &value3);
            write(socket_cliente, &result, sizeof(int));
            if (result == 1) {
                write(socket_cliente, value1, 255);
                write(socket_cliente, &value2, sizeof(int));
                write(socket_cliente, &value3, sizeof(double));
            }
            break;
        case 3:
            read(socket_cliente, &clave, sizeof(int));
            read(socket_cliente, value1, 255);
            read(socket_cliente, &value2, sizeof(int));
            read(socket_cliente, &value3, sizeof(double));
            result = modify_value(clave, value1, value2, value3);
            write(socket_cliente, &result, sizeof(int));
            break;
        case 4:
            read(socket_cliente, &clave, sizeof(int));
            result = delete_key(clave);
            write(socket_cliente, &result, sizeof(int));
            break;
        case 5:
            read(socket_cliente, &clave, sizeof(int));
            result = exist(clave);
            write(socket_cliente, &result, sizeof(int));
            break;
        case 6:
            read(socket_cliente, &clave, sizeof(int));
            read(socket_cliente, &value2, sizeof(int));
            result = copy_key(clave, value2);
            write(socket_cliente, &result, sizeof(int));
            break;
    }
}

int main() {
    int socket_servidor, socket_cliente, addrlen;
    struct sockaddr_in address;

    socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_servidor == 0) {
        perror("Error en socket()");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(socket_servidor, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Error en bind()");
        exit(EXIT_FAILURE);
    }

    if (listen(socket_servidor, 10) < 0) {
        perror("Error en listen()");
        exit(EXIT_FAILURE);
    }

    printf("Servidor iniciado. Escuchando en el puerto %d...\n", PORT);
    addrlen = sizeof(address);

    while (1) {
        if ((socket_cliente = accept(socket_servidor, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
            perror("Error en accept()");
            exit(EXIT_FAILURE);
        }

        printf("Nueva conexión aceptada. Socket del cliente: %d\n", socket_cliente);

        procesar_peticion(socket_cliente);

        close(socket_cliente);
    }

    return 0;
}

