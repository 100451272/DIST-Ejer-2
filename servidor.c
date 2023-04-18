
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "list.h"

// declare a global mutex
pthread_mutex_t mutex_mensaje, mutex_lista;
pthread_cond_t c_cop;
bool copiado = false;
List list;
int sd, sc;

char* respuesta_to_char(struct peticion p) {
    char* serializado = malloc(1024);
    char tmp[1024];
    sprintf(tmp, "%d/%d/%s/%d/%f", p.op, p.tupla.clave, p.tupla.valor1, p.tupla.valor2, p.tupla.valor3);
    strcpy(serializado, tmp);
    return serializado;
}

struct peticion char_to_peticion(char* serialized) {
    struct peticion *p = (struct peticion *) malloc(sizeof(struct peticion));
    sscanf(serialized, "%d/%d/%[^/]/%d/%lf", &p->op, &p->tupla.clave, p->tupla.valor1, &p->tupla.valor2, &p->tupla.valor3);
    return *p;
}

void petition_handler(void *peticion){
    struct peticion res;
    bzero((char *)&res, sizeof(res));
    // Protegemos la copia de la peticion con mutex
    pthread_mutex_lock(&mutex_mensaje);
    struct peticion pet = (*(struct peticion *)peticion);
    copiado = true;
    int client_socket = sc;
    pthread_cond_signal(&c_cop);
    pthread_mutex_unlock(&mutex_mensaje);

    // Protegemos las operaciones sobre la lista para evitar
    // condiciones de carrera en la lista
    pthread_mutex_lock(&mutex_lista);
    switch (pet.op) {
        case 0: //INIT
            init(&list);
            res.op= 0;
            break;

        case 1: //SET_VALUE
            set(&list, pet.tupla.clave, &pet.tupla);
            res.op = 0;
            printList(list);
            break;

        case 2: //GET_VALUE
            res.op = get(&list, pet.tupla.clave, &pet.tupla);
            if (res.op == 0) { // si la operación fue exitosa
                // copiamos los valores de la tupla a los campos de la respuesta
                strcpy(res.tupla.valor1, pet.tupla.valor1);
                res.tupla.valor2 = pet.tupla.valor2;
                res.tupla.valor3 = pet.tupla.valor3;
            }
            break;

        case 3: //MODIFY_VALUE
        if (exists(&list, pet.tupla.clave) == 0) {
            printf("La clave %d no existe\n", pet.tupla.clave);
            res.op = -1;
            break;
        }
            delete_node(&list, pet.tupla.clave);
            set(&list, pet.tupla.clave, &pet.tupla);
            printList(list);
            res.op = 0;
            break;

        case 4: //DELETE_KEY
            if (delete_node(&list, pet.tupla.clave) == 0) {
                res.op = 0;
            } else {
                res.op = -1;
            }
            printList(list);
            fflush(NULL);
            break;

        case 5: //EXIST
            res.op = exists(&list, pet.tupla.clave);
            if (res.op == 0) {
                printf("No existe\n");
            }
            else if (res.op == 1){
                printf("Existe\n");
            }
            printList(list);
            break;

        case 6: // COPY_KEY
        {
            struct tupla tupla1, tupla2;
            int key1 = pet.tupla.clave;
            int key2 = pet.tupla.valor2;

            // Buscar la tupla de key1
            if (get(&list, key1, &tupla1) == -1) {
                res.op = -1; // La clave key1 no existe
                break;
            }

            // Crear la tupla de key2 con los valores de key1
            tupla2.clave = key2;
            strcpy(tupla2.valor1, tupla1.valor1);
            tupla2.valor2 = tupla1.valor2;
            tupla2.valor3 = tupla1.valor3;

            // Insertar o modificar la tupla en la lista
            delete_node(&list, tupla2.clave);
            set(&list, key2, &tupla2);

            res.op = 0;
            printList(list);
            break;
        }
    }
    // Liberamos el mutex de la lista para que puedan acceder otros
    // threads a la lista
    pthread_mutex_unlock(&mutex_lista);
    char* buffer = respuesta_to_char(res);
    int sent = send(client_socket, buffer, 1024, 0);
    if (sent == -1){
        printf("Error en el send\n");
    }
    close(client_socket);
    
    pthread_exit(0);
}



int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Error. Uso: ./servidor <PUERTO>\n");
        return -1;
    }
    int puerto = atoi(argv[1]);
    struct peticion pet;
    pthread_attr_t t_attr;
   	pthread_t thid;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);

    
    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int val = 1;
    int err = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));
    if (err < 0){
        printf("Error en option");
        exit(-1);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(puerto);
    server_address.sin_addr.s_addr = INADDR_ANY;
    bind(sd, (struct sockaddr*)&server_address, sizeof(server_address));


    
    pthread_mutex_init(&mutex_mensaje, NULL);
	pthread_cond_init(&c_cop, NULL);
    pthread_mutex_init(&mutex_lista, NULL);
	pthread_attr_init(&t_attr);

	pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    listen(sd, SOMAXCONN);

    while(1) {
        sc = accept(sd, (struct sockaddr*)&client_address, &client_address_length);
        char* buffer = malloc(1024);
        int received = recv(sc, buffer, 1024, 0);
        if (received < 0){
            printf("Error en la recepción\n");
        }
        fflush(NULL);
        pet = char_to_peticion(buffer);
        printf("Peticion recibida: %d\n", pet.op);
        copiado = false;
        pthread_mutex_lock(&mutex_mensaje);
        pthread_create(&thid, &t_attr, (void *)petition_handler, (void *)&pet);
        while (!copiado){
            pthread_cond_wait(&c_cop, &mutex_mensaje);
        }
        copiado = true;
        pthread_mutex_unlock(&mutex_mensaje);  
        fflush(NULL);
        }   
    close(sd);
    return 0;
}