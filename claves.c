#include <stdio.h>
#include <stdlib.h>
#include "claves.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>

#define PORT_TUPLAS 8080
#define IP_TUPLAS "127.0.0.1"

int init(void) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_TUPLAS);
    if (inet_pton(AF_INET, IP_TUPLAS, &serv_addr.sin_addr) <= 0) {
        perror("Error al establecer la dirección del servidor");
        exit(EXIT_FAILURE);
    }
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error al conectar al servidor");
        exit(EXIT_FAILURE);
    }
    struct peticion pet;
    struct peticion res;
    pet.op = 0;

    if (send(sockfd, (const char *) &pet, sizeof(pet), 0) < 0) {
        perror("Error al enviar datos al servidor");
        exit(EXIT_FAILURE);
    }

    if (recv(sockfd, (char *) &res, sizeof(res), 0) < 0) {
        perror("Error al recibir datos del servidor");
        exit(EXIT_FAILURE);
    }

    printf("Resultado = %d\n", res.op);

    close(sockfd);
    return res.op;
}


int modify_value(int key, char *value1, int value2, double value3){
    int sockfd;
    struct sockaddr_in servaddr;
    char recvline[1024];
    struct peticion pet;
    int op;
    // Obtener la dirección IP y el puerto del servidor
    char *ip_tuplas = getenv("IP_TUPLAS");
    char *port_tuplas = getenv("PORT_TUPLAS");
    if (ip_tuplas == NULL || port_tuplas == NULL) {
        perror("No se encontraron las variables de entorno IP_TUPLAS y PORT_TUPLAS");
        return -1;
    }
    // Crear un socket para conectarse al servidor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("No se pudo crear el socket");
        return -1;
    }
    // Configurar la dirección del servidor
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, ip_tuplas, &servaddr.sin_addr) <= 0) {
        perror("Error en la dirección del servidor");
        close(sockfd);
        return -1;
    }
    if (port_tuplas == NULL) {
        perror("No se encontró la variable de entorno PORT_TUPLAS");
        close(sockfd);
        return -1;
    }
    // Convertir el puerto a un número entero
    int port = atoi(port_tuplas);
    if (port <= 0) {
        perror("Error en el puerto del servidor");
        close(sockfd);
        return -1;
    }
    servaddr.sin_port = htons(port);
    // Conectar al servidor
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("No se pudo conectar al servidor");
        close(sockfd);
        return -1;
    }
    // Rellenar la petición
    pet.op = 3;
    pet.tupla.clave = key;
    strcpy(pet.tupla.valor1, value1);
    pet.tupla.valor2 = value2;
    pet.tupla.valor3 = value3;
    // Enviar la petición al servidor
    if (write(sockfd, &pet, sizeof(pet)) < 0) {
        perror("No se pudo enviar la petición al servidor");
        close(sockfd);
        return -1;
    }
    // Leer la respuesta del servidor
    if (read(sockfd, &op, sizeof(op)) < 0) {
        perror("No se pudo leer la respuesta del servidor");
        close(sockfd);
        return -1;
    }
    printf("Resultado = %d\n", op);
    // Cerrar el socket
    close(sockfd);
    return op;
}




int set_value(int key, char *value1, int value2, double value3){
    int sock = 0;
    struct sockaddr_in serv_addr;
    struct peticion pet;
    struct peticion res;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket creation error");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_TUPLAS);

    if(inet_pton(AF_INET, IP_TUPLAS, &serv_addr.sin_addr)<=0){
        perror("invalid address");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perror("connection failed");
        return -1;
    }

    /* se rellena la petición */
    struct tupla tupla;
    tupla.clave = key;
    strcpy(tupla.valor1, value1);
    tupla.valor2 = value2;
    tupla.valor3 = value3;

    pet.op = 1;
    pet.tupla = tupla;

    if (send(sock, (char *) &pet, sizeof(pet), 0) < 0){
        perror("send");
        return -1;
    }

    if (recv(sock, (char *) &res, sizeof(res), 0) < 0){
        perror("recv");
        return -1;
    }

    printf("Resultado = %d\n", res.op);

    close(sock);
    return res.op;
}



int get_value(int key, char *value1, int *value2, double *value3){
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    struct peticion pet;
    struct peticion res;

    /* se crea el socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    /* se configura la dirección del servidor */
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_TUPLAS);
    if (inet_pton(AF_INET, IP_TUPLAS, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        return -1;
    }

    /* se conecta con el servidor */
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        return -1;
    }

    /* se rellena la petición */
    pet.op = 2;
    pet.tupla.clave = key;

    /* se envía la petición al servidor */
    if (send(sock, &pet, sizeof(pet), 0) < 0) {
        perror("send");
        return -1;
    }

    /* se recibe la respuesta del servidor */
    if (recv(sock, &res, sizeof(res), 0) < 0) {
        perror("recv");
        return -1;
    }

    /* se cierra la conexión */
    close(sock);

    /* se copian los valores de la tupla */
    strcpy(value1, res.tupla.valor1);
    *value2 = res.tupla.valor2;
    *value3 = res.tupla.valor3;

    printf("Resultado = %d\n", res.op);

    return res.op;
}


int delete_key(int key){
    int sockfd, n;
    struct sockaddr_in serv_addr;
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_TUPLAS);

    if (inet_pton(AF_INET, IP_TUPLAS, &serv_addr.sin_addr) <= 0) {
        perror("ERROR inet_pton");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        close(sockfd);
        return -1;
    }

    /* se rellena la petición */
    struct tupla tupla;
    tupla.clave = key;
    strcpy(tupla.valor1, "");
    tupla.valor2 = 0;
    tupla.valor3 = 0.0;

    struct peticion pet;
    pet.op = 4;
    pet.tupla = tupla;

    bzero(buffer, 256);
    memcpy(buffer, &pet, sizeof(pet));
    n = write(sockfd, buffer, sizeof(buffer));
    if (n < 0) {
        perror("ERROR writing to socket");
        close(sockfd);
        return -1;
    }

    bzero(buffer, 256);
    n = read(sockfd, buffer, sizeof(buffer));
    if (n < 0) {
        perror("ERROR reading from socket");
        close(sockfd);
        return -1;
    }

    struct peticion res;
    memcpy(&res, buffer, sizeof(buffer));
    printf("Resultado = %d\n", res.op);

    close(sockfd);
    return res.op;
}


int exist(int key){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_TUPLAS);
    if (inet_pton(AF_INET, IP_TUPLAS, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    struct peticion pet;
    struct respuesta res;

    pet.op = 5;
    pet.tupla.clave = key;

    if (send(sockfd, &pet, sizeof(pet), 0) < 0) {
        perror("send");
        close(sockfd);
        return -1;
    }

    if (recv(sockfd, &res, sizeof(res), 0) < 0) {
        perror("recv");
        close(sockfd);
        return -1;
    }

    printf("Resultado = %d\n", res.op);

    close(sockfd);
    return res.op;
}


int copy_key(int key1, int key2) {
    int sockfd;
    struct sockaddr_in servaddr;
    struct peticion pet;
    struct respuesta res;

    // Creamos el socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // Configuramos la dirección del servidor
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT_TUPLAS);
    if (inet_pton(AF_INET, IP_TUPLAS, &servaddr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        return -1;
    }

    // Establecemos la conexión con el servidor
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    // Preparamos la petición
    pet.op = 6;
    pet.tupla.clave = key1;
    pet.tupla.valor2 = key2;

    // Enviamos la petición al servidor
    if (write(sockfd, &pet, sizeof(pet)) < 0) {
        perror("write");
        close(sockfd);
        return -1;
    }

    // Recibimos la respuesta del servidor
    if (read(sockfd, &res, sizeof(res)) < 0) {
        perror("read");
        close(sockfd);
        return -1;
    }

    printf("Resultado = %d\n", res.op);

    // Cerramos la conexión y devolvemos el resultado
    close(sockfd);
    return res.op;
}