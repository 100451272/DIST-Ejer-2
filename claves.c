
#include <stdio.h>
#include <stdlib.h>
#include "claves.h"
#include <string.h>
#include <mqueue.h>

struct peticion send_recieve(struct peticion pet){
    mqd_t q_servidor;
    mqd_t q_cliente;

    struct peticion res;
    struct mq_attr attr;

    attr.mq_maxmsg = 10;     attr.mq_msgsize = sizeof(res);
    q_cliente = mq_open("/CLIENTE", O_CREAT|O_RDONLY, 0700, &attr);
	if (q_cliente == -1) {
		perror("mq_open");
		exit(-1);
	}
    q_servidor = mq_open("/ALMACEN", O_WRONLY);
	if (q_servidor == -1){
		mq_close(q_cliente);
		perror("mq_open");
		exit(-1);
	}
    if (mq_send(q_servidor, (const char *)&pet, sizeof(pet), 0) < 0){
		perror("mq_send");
		exit(-1);
    }	
    if (mq_receive(q_cliente, (char *)&res, sizeof(res), 0) < 0){
		perror("mq_recv");
		exit(-1);
    }	

    printf("Resultado = %d\n", res.op);

    mq_close(q_servidor);
    mq_close(q_cliente);
    mq_unlink("/CLIENTE");
    return res;
}

int init(void){
    struct peticion pet;
    struct peticion res;
    /* se rellena la petición */
    pet.op = 0; 
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
	res = send_recieve(pet);
	return res.op;
}

int exist(int key){
    struct peticion pet;
    struct peticion res;
    /* se rellena la petición */
    pet.op = 5;
    pet.tupla.clave = key;
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
	res = send_recieve(pet);
	return res.op;
}
