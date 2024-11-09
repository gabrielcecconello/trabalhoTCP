#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include "../utils/msg_utils.h"

#define PORT 8080
#define MAX_TEXT_LEN 140 // Limite máximo de caracteres para 'text'

int main(int argc, char const *argv[])
{
    int valread, client_fd;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client"; // Exemplo de mensagem
    char buffer[BUFFER_SIZE] = {0};

    // Verificar se foi fornecido um identificador de cliente
    if (argc != 2)
    {
        printf("Uso: %s <ID do Cliente (1001-1999)>\n", argv[0]);
        return -1;
    }

    // Usar atoi para converter o ID do cliente
    unsigned short int client_id = atoi(argv[1]);

    // Verificar se o ID do cliente está no intervalo de 1001 a 1999
    if (client_id < 1001 || client_id > 1999)
    {
        printf("ID do cliente inválido. O ID deve estar entre 1001 e 1999.\n");
        return -1;
    }

    printf("ID do cliente de envio: %d\n", client_id);

    // Criar o socket TCP
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

    // Conectar ao servidor
    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nFalha na conexão\n");
        return -1;
    }

    // Garantir que a string não ultrapasse o tamanho máximo permitido (MAX_TEXT_LEN)
    char message[MAX_TEXT_LEN + 1];
    strncpy(message, hello, MAX_TEXT_LEN); // Limitar a string ao tamanho máximo
    message[MAX_TEXT_LEN] = '\0';          // Garantir que a string é terminada com '\0'

    // Construir a mensagem OI com o ID do cliente
    msg_t message_struct;
    // initialize_msg(&message_struct); // Inicialize a estrutura de mensagem

    fill_msg(&message_struct, 2, client_id, 0, (unsigned char *)message);
    send_msg(client_fd, &message_struct);

    printf("Mensagem %s enviada para o servidor\n", hello);
    fflush(stdout);

    printf("\n\n");
    printf("Resposta do servidor:\n");
    receive_msg(client_fd, &message_struct);
    print_msg(&message_struct);

    fflush(stdout);
    close(client_fd);
    return 0;
}
