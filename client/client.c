#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "../utils/msg_utils.h"

int main(int argc, char const *argv[]) {
    msg_t message;
    int client_fd, message_destination;
    struct sockaddr_in serv_addr;
    unsigned char buffer[BUFFER_SIZE];
    char message_str[100];

    bool is_sending_message = true;
    bool is_first_message = true;

    // Verificar se foi fornecido um identificador de cliente (argumento da linha de comando)
    if (argc != 2) {
        printf("Uso: %s <ID do Cliente (1001-1999)>\n", argv[0]);
        return -1;
    }

    // Usar atoi para converter o ID do cliente (agora funciona corretamente)
    unsigned short int client_id = atoi(argv[1]);

    // Verificar se o ID é válido (1001-1999)
    if (client_id < 1001 || client_id > 1999) {
        printf("Error: ID do cliente inválido. O ID deve ser entre 1001 e 1999.\n");
        return -1;
    }

    system("clear");
    printf("\t\t\t*** Cliente %d ***\n", client_id);

    // Cria um socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nErro ao criar socket\n");
        return -1;
    }

    // Configurar as informações do servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converter o endereço IP do servidor (usando 127.0.0.1 para localhost)
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nEndereço inválido\n");
        return -1;
    }

    // Conecta com o servidor
    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nFalha na conexão\n");
        return -1;
    }

    // Envio de mensagem OI
    fill_msg(&message, 0, client_id, 0, "OI");
    send_msg(client_fd, &message);
    printf("Conectando ao servidor ... \n\n");

    // Leitura da resposta do servidor
    if(!receive_msg(client_fd, &message)) {
        printf("Erro: conexao rejeitada do servidor\n\n");
        return 0;
    }

    printf("\nMensagem do servidor: %s\n\n", message.text);

    if(message.type == 1) {
        printf("\n\n");
        close(client_fd);
        return 0;
    }
    
    // Envio de multiplas mensagens para o servidor
    while(is_sending_message) {
        if(!is_first_message) {
            printf("\t\t\t*** Cliente %d ***\n\n", client_id);
        }

        printf("Insira uma mensagem ou [q] para parar\n\n");
        printf("mensagem: ");

        if(!is_first_message) {
            getchar();
        }
        
        scanf("%[^\n]", message_str);
        fflush(stdout);

        if(strcmp(message_str, "q") != 0) {
            printf("Identificacao de destino: ");
            scanf("%d", &message_destination);
            fflush(stdout);

            fill_msg(&message, 2, client_id, message_destination, message_str);
            send_msg(client_fd, &message);
        } else {
            is_sending_message = false;
        }

        is_first_message = false;
        system("clear");
    }

    // Envio de mensagem TCHAU e desconexao com o servidor
    printf("\t\t\t*** Cliente %d ***\n\nDesligando sistema ...\n\n", client_id);
    fill_msg(&message, 1, client_id, client_id - 1000, "TCHAU");
    send_msg(client_fd, &message);

    fflush(stdout);
    close(client_fd);
    return 0;
}
