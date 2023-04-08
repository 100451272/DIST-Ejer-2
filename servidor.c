#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "claves.h"

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

    // Obtener la dirección IP del servidor de tuplas desde la variable de entorno
    char *ip_tuplas = getenv("IP_TUPLAS");
    if (ip_tuplas == NULL) {
        printf("La variable de entorno IP_TUPLAS no está definida.\n");
        exit(EXIT_FAILURE);
    }

    // Obtener el puerto del servidor de tuplas desde la variable de entorno
    char *port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL) {
        printf("La variable de entorno PORT_TUPLAS no está definida.\n");
        exit(EXIT_FAILURE);
    }

    socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_servidor == 0) {
        perror("Error en socket()");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip_tuplas);  // Utilizar la dirección IP obtenida de la variable de entorno
    address.sin_port = htons(atoi(port_tuplas));  // Utilizar el puerto obtenido de la variable de entorno

    if (bind(socket_servidor, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Error en bind()");
        exit(EXIT_FAILURE);
    }

    if (listen(socket_servidor, 10) < 0) {
        perror("Error en listen()");
        exit(EXIT_FAILURE);
    }

    printf("Servidor iniciado. Escuchando en el puerto %s...\n", port_tuplas);
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
