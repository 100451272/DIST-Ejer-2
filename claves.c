#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include "claves.h"
#include <string.h>
#include <mqueue.h>

char* peticion_to_char(struct peticion p) {
    char* serializado = malloc(1024);
    char tmp[1024];
    sprintf(tmp, "%d/%d/%s/%d/%f", p.op, p.tupla.clave, p.tupla.valor1, p.tupla.valor2, p.tupla.valor3);
    strcpy(serializado, tmp);
    return serializado;
}

struct peticion char_to_respuesta(char* serialized) {
    struct peticion *p = (struct peticion *) malloc(sizeof(struct peticion));
    sscanf(serialized, "%d/%d/%[^/]/%d/%lf", &p->op, &p->tupla.clave, p->tupla.valor1, &p->tupla.valor2, &p->tupla.valor3);
    return *p;
}

struct peticion send_recieve(struct peticion pet){
    char *ip;
    ip = getenv("IP_TUPLAS");
    if (ip == NULL){
        printf("Variable IP_TUPLAS no definida\n");
        exit(-1);
    }
    char *port;
    port = getenv("PORT_TUPLAS");
    if (port == NULL){
        printf("Variable PORT_TUPLAS no definida\n");
        exit(-1);
    }
    // Comenzamos a definir el socket a conectar
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0){
            perror("Error in socket");
            exit(1);
    }

	// Obtain Server address 
    
    bzero((char *)&server_addr, sizeof(server_addr));
    hp = gethostbyname ( ip);
    if (hp == NULL) {
            perror("Error en gethostbyname");
            exit(1);
    }

	memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));

    
    if (connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Error al conectar al servidor");
        exit(EXIT_FAILURE);
    }
    char* buffer = malloc(1024);
    buffer = peticion_to_char(pet);
    if (send(sd, (const char *) buffer, strlen(buffer), 0) < 0) {
        perror("Error al enviar datos al servidor");
        exit(EXIT_FAILURE);
    }
    char* buffer_resp = malloc(1024);
    if (recv(sd, (char *) buffer_resp, 1024, 0) < 0) {
        perror("Error al recibir datos del servidor");
        fflush(NULL);
        exit(EXIT_FAILURE);
    }
    struct peticion res = char_to_respuesta(buffer_resp);

    printf("Resultado = %d\n", res.op);
    close(sd);
    

    return res;
}

int init(void){
    struct peticion pet;
    struct peticion res;
    /* se rellena la petición */
    pet.op = 0; 
    pet.tupla.clave = 0;
    strcpy(pet.tupla.valor1, "0");
    pet.tupla.valor2 = 0;
    pet.tupla.valor3 = 0;
	res = send_recieve(pet);
	return res.op;
}

int set_value(int key, char *value1, int value2, double value3){

    struct peticion pet;
    struct peticion res;
    /* se rellena la petición */
    struct tupla tupla;
    tupla.clave = key;
    strcpy(tupla.valor1, value1);
    tupla.valor2 = value2;
    tupla.valor3 = value3;

    pet.op = 1;
    pet.tupla = tupla;

    res = send_recieve(pet);
	return res.op;
}

int get_value(int key, char *value1, int *value2, double *value3){
    struct peticion pet;
    struct peticion res;
    /* se rellena la petición */
    pet.op = 2;
    pet.tupla.clave = key;
    strcpy(pet.tupla.valor1, "0");
    pet.tupla.valor2 = 0;
    pet.tupla.valor3 = 0;
	res = send_recieve(pet);
    strcpy(value1, res.tupla.valor1);
    *value2 = res.tupla.valor2;
    *value3 = res.tupla.valor3;
	return res.op;
}

int modify_value(int key, char *value1, int value2, double value3){
    struct peticion pet;
    struct peticion res;
    /* se rellena la petición */
    pet.op = 3;
    pet.tupla.clave = key;
    strcpy(pet.tupla.valor1, value1);
    pet.tupla.valor2 = value2;
    pet.tupla.valor3 = value3;
	res = send_recieve(pet);
	return res.op;   
}


int delete_key(int key){
    struct peticion pet;
    struct peticion res;
    /* se rellena la petición */
    pet.op = 4;
    pet.tupla.clave = key;
    strcpy(pet.tupla.valor1, "0");
    pet.tupla.valor2 = 0;
    pet.tupla.valor3 = 0;
	res = send_recieve(pet);
	return res.op;
}

int exist(int key){
    struct peticion pet;
    struct peticion res;
    /* se rellena la petición */
    pet.op = 5;
    pet.tupla.clave = key;
    strcpy(pet.tupla.valor1, "0");
    pet.tupla.valor2 = 0;
    pet.tupla.valor3 = 0;
	res = send_recieve(pet);
	return res.op;
}

int copy_key(int key1, int key2){
    struct peticion pet;
    struct peticion res;
    /* se rellena la petición */
    pet.tupla.clave = key1;
    pet.tupla.valor2 = key2; // Reutilizamos el valor 2 como paso de la clave 2
    pet.op = 6;
    strcpy(pet.tupla.valor1, "0");
    pet.tupla.valor3 = 0;
	res = send_recieve(pet);
	return res.op;
}
