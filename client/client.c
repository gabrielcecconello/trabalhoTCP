#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include<stdlib.h>
#include <stdio.h>

#include "../utils/msg_utils.h"

int main(int argc, char const *argv[]) {
    msg_t mensage;
    int valread, client_fd, mensage_destination;
    struct sockaddr_in serv_addr;
    unsigned char buffer[BUFFER_SIZE];
    char mensage_str[100];

    bool is_sending_mensage = true;
    bool is_fisrt_mensage = true;

    system("clear");
    printf("\t\t\t*** Cliente {id} ***\n");

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Erro ao criar socket \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf(" \n Endereço Inválido \n");
        return -1;
    }

    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Falha na conexão \n");
        return -1;
    }

    fill_msg(&mensage, 0, 0, 0, "OI");
    send_msg(client_fd, &mensage);
    printf("Conectando ao servidor ... \n\n");

    receive_msg(client_fd, &mensage);
    printf("\nMensagem do servidor: %s\n\n", mensage.text);

    while(is_sending_mensage) {
        if(!is_fisrt_mensage) {
            printf("\t\t\t*** Cliente {id} ***\n\n");
        }

        printf("Insira uma mensagem ou [q] para parar\n\n");
        printf("mensagem: ");

        if(!is_fisrt_mensage) {
            getchar();
        }
        
        scanf("%[^\n]", mensage_str);
        fflush(stdout);

        if(strcmp(mensage_str, "q") != 0) {
            printf("Identificacao de destino: ");
            scanf("%d", &mensage_destination);
            fflush(stdout);

            fill_msg(&mensage, 2, 0, mensage_destination, mensage_str);
            send_msg(client_fd, &mensage);
        } else {
            is_sending_mensage = false;
        }

        is_fisrt_mensage = false;
        system("clear");
    }

    printf("\t\t\t*** Cliente {id} ***\n\nDesligando sistema ...\n\n");
    fill_msg(&mensage, 1, 0, 0, "TCHAU");
    send_msg(client_fd, &mensage);

    close(client_fd);
    return 0;
}