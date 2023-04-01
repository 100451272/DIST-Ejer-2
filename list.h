
#ifndef P_LIST_H
#define P_LIST_H        1

#define MAX_KEY_LENGTH	256

struct Node{ 
	int 	key;
	struct  tupla *tupla;
	struct 	Node *next; 
};

struct tupla {
    int clave;
    char valor1[255];
    int valor2;
    double valor3;
};

struct peticion{
    int op;
    struct tupla tupla;
};

typedef struct Node * List;


int init(List *l);
int exists(List *l, int key);
int set(List *l, int key, struct tupla *tupla);
int get(List *l, int key, struct tupla *tupla);
int printList(List l);
int delete_node(List *l, int key);
int destroy(List *l);

#endif //P_LIST_H
