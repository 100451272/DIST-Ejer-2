#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void procesar_peticion(int sockfd){
    int op, key, value2;
    char value1[256];
    double value3;
    struct tupla tupla;

    if(recv(sockfd, &op, sizeof(op), 0) < 0){
        perror("recv");
        exit(-1);
    }

    switch(op){
        case 0: // init()
            key = init();
            if(send(sockfd, &key, sizeof(key), 0) < 0){
                perror("send");
                exit(-1);
            }
            break;
        case 1: // set_value(int key, char *value1, int value2, double value3)
            if(recv(sockfd, &key, sizeof(key), 0) < 0 ||
               recv(sockfd, value1, sizeof(value1), 0) < 0 ||
               recv(sockfd, &value2, sizeof(value2), 0) < 0 ||
               recv(sockfd, &value3, sizeof(value3), 0) < 0){
                perror("recv");
                exit(-1);
            }

            if(set_value(key, value1, value2, value3)){
                if(send(sockfd, &OK, sizeof(OK), 0) < 0){
                    perror("send");
                    exit(-1);
                }
            }else{
                if(send(sockfd, &ERROR, sizeof(ERROR), 0) < 0){
                    perror("send");
                    exit(-1);
                }
            }
            break;
        case 2: // get_value(int key, char *value1, int *value2, double *value3)
            if(recv(sockfd, &key, sizeof(key), 0) < 0){
                perror("recv");
                exit(-1);
            }

            if(get_value(key, value1, &value2, &value3)){
                if(send(sockfd, &OK, sizeof(OK), 0) < 0 ||
                   send(sockfd, value1, sizeof(value1), 0) < 0 ||
                   send(sockfd, &value2, sizeof(value2), 0) < 0 ||
                   send(sockfd, &value3, sizeof(value3), 0) < 0){
                    perror("send");
                    exit(-1);
                }
            }else{
                if(send(sockfd, &ERROR, sizeof(ERROR), 0) < 0){
                    perror("send");
                    exit(-1);
                }
            }
            break;
        case 3: // modify_value(int key, char *value1, int value2, double value3)
            if(recv(sockfd, &key, sizeof(key), 0) < 0 ||
               recv(sockfd, value1, sizeof(value1), 0) < 0 ||
               recv(sockfd, &value2, sizeof(value2), 0) < 0 ||
               recv(sockfd, &value3, sizeof(value3), 0) < 0){
                perror("recv");
                exit(-1);
            }

            if(modify_value(key, value1, value2, value3)){
                if(send(sockfd, &OK, sizeof(OK), 0) < 0){
                    perror("send");
                    exit(-1);
                }
            }else{
                if(send(sockfd, &ERROR, sizeof(ERROR), 0) < 0){
                    perror("send");
                    exit(-1);
                }
            }
            break;
        case 4: // delete_key(int key)
            if(recv(sockfd, &key, sizeof(key), 0)








int main() {
    int server_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length = sizeof(client_address);

    // Crear el socket del servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Asignar la dirección al socket del servidor
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error al asignar la dirección al socket del servidor");
        exit(EXIT_FAILURE);
    }

    // Esperar conexiones de los clientes
    while (1) {
        int client_socket;
        char buffer[MAX_MESSAGE_SIZE] = {0};

        // Escuchar por conexiones de los clientes
        if (listen(server_socket, MAX_CLIENTS) < 0) {
            perror("Error al esperar por conexiones de los clientes");
            exit(EXIT_FAILURE);
        }

        // Aceptar una conexión de un cliente
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length);
        if (client_socket < 0) {
            perror("Error al aceptar una conexión de un cliente");
            exit(EXIT_FAILURE);
        }

        // Leer el mensaje del cliente
        read(client_socket, buffer, MAX_MESSAGE_SIZE);
        printf("Mensaje recibido del cliente: %s\n", buffer);

        // Procesar la petición y enviar la respuesta al cliente
        char respuesta[MAX_MESSAGE_SIZE] = {0};
        procesar_peticion(buffer, respuesta);
        write(client_socket, respuesta, strlen(respuesta));
        printf("Respuesta enviada al cliente: %s\n", respuesta);

        // Cerrar el socket del cliente
        close(client_socket);
    }

    // Cerrar el socket del servidor
    close(server_socket);

    return 0;
}
