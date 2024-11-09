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

    // Verificar se foi fornecido um identificador de cliente (argumento da linha de comando)
    if (argc != 2) {
        printf("Uso: %s <ID do Cliente (1001-1999)>\n", argv[0]);
        return -1;
    }

    // Usar atoi para converter o ID do cliente (agora funciona corretamente)
    unsigned short int client_id = atoi(argv[1]);

    // Verificar se o ID é válido (1-999)
    if (client_id < 1001 || client_id > 1999) {
        printf("Error: ID do cliente inválido. O ID deve ser entre 1001 e 1999.\n");
        return -1;
    }

    system("clear");
    printf("\t\t\t*** Cliente %d ***\n", client_id);

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

    fill_msg(&mensage, 0, client_id, 0, "OI");
    send_msg(client_fd, &mensage);
    printf("Conectando ao servidor ... \n\n");

    receive_msg(client_fd, &mensage);
    printf("\nMensagem do servidor: %s\n\n", mensage.text);

    while(is_sending_mensage) {
        if(!is_fisrt_mensage) {
            printf("\t\t\t*** Cliente %d ***\n\n", client_id);
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

            fill_msg(&mensage, 2, client_id, mensage_destination, mensage_str);
            send_msg(client_fd, &mensage);
        } else {
            is_sending_mensage = false;
        }

        is_fisrt_mensage = false;
        system("clear");
    }

    printf("\t\t\t*** Cliente %d ***\n\nDesligando sistema ...\n\n", client_id);
    fill_msg(&mensage, 1, client_id, 0, "TCHAU");
    send_msg(client_fd, &mensage);

    close(client_fd);
    return 0;
}