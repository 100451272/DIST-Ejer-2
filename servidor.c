
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "list.h"

// declare a global mutex
pthread_mutex_t mutex_mensaje, mutex_lista;
pthread_cond_t c_cop;
bool copiado;
List list;
mqd_t q_servidor;

void petition_handler(void *peticion){
    mqd_t q_cliente;
    struct peticion res;
    // Protegemos la copia de la peticion con mutex
    pthread_mutex_lock(&mutex_mensaje);
    struct peticion pet = (*(struct peticion *)peticion);
    copiado = true;
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
    q_cliente = mq_open("/CLIENTE", O_WRONLY);
		if (q_cliente < 0) {
			perror("mq_open");
			mq_close(q_servidor);
			mq_unlink("/ALMACEN");
		}
        else {
            if (mq_send(q_cliente, (const char *)&res, sizeof(res), 0) < 0) {
                perror("mq_send");
                mq_close(q_servidor);
                mq_unlink("/ALMACEN");
                mq_close(q_cliente);
            }
            fflush(NULL);
        }
    pthread_exit(0);
}



int main(void){
    struct peticion pet;
    struct mq_attr attr;
    pthread_attr_t t_attr;
   	pthread_t thid;

    attr.mq_maxmsg = 10;                
	attr.mq_msgsize = sizeof(struct peticion);
    q_servidor = mq_open("/ALMACEN", O_CREAT|O_RDONLY, 0700, &attr);
	if (q_servidor == -1) {
		perror("mq_open");
		return -1;
	}
    pthread_mutex_init(&mutex_mensaje, NULL);
	pthread_cond_init(&c_cop, NULL);
    pthread_mutex_init(&mutex_lista, NULL);
	pthread_attr_init(&t_attr);

	pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

        while(1) {
        if (mq_receive(q_servidor, (char *) &pet, sizeof(pet), 0) < 0){
			perror("mq_recev");
			return -1;
		}
        printf("Peticion recibida: %d\n", pet.op);
        if (pthread_create(&thid, &t_attr, (void *)petition_handler, (void *)&pet)== 0) {
			// se espera a que el thread copie el mensaje 
			pthread_mutex_lock(&mutex_mensaje);
			while (!copiado)
				pthread_cond_wait(&c_cop, &mutex_mensaje);
			copiado = false;
			pthread_mutex_unlock(&mutex_mensaje);
	 	}  
        fflush(NULL);
        //printf("%d", res.op);
                /* se responde al cliente abriendo reviamente su cola */
        }   
    mq_close(q_servidor);
    // Remove the message queue from the system
    mq_unlink("/ALMACEN");
    return 0;
}