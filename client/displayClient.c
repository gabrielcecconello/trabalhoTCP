#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../utils/msg_utils.h"

int main(int argc, char const *argv[])
{
    int valread, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0}; // Buffer para armazenar a mensagem recebida do servidor

    // Verificar se foi fornecido um identificador de cliente (argumento da linha de comando)
    if (argc != 2)
    {
        printf("Uso: %s <ID do Cliente (1-999)>\n", argv[0]);
        return -1;
    }

    // Usar atoi para converter o ID do cliente (agora funciona corretamente)
    unsigned short int client_id = atoi(argv[1]);

    // Verificar se o ID é válido (1-999)
    if (client_id < 1 || client_id > 999)
    {
        printf("ID do cliente inválido. O ID deve ser entre 1 e 999.\n");
        return -1;
    }

    system("clear");
    printf("\t\t\t*** Cliente %d ***\n", client_id);

    // Criar o socket TCP
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Erro ao criar o socket");
        return -1;
    }

    // Configurar as informações do servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT); // A porta do servidor (8080)

    // Converter o endereço IP do servidor (aqui, usaremos "127.0.0.1" para localhost)
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Endereço inválido ou não suportado");
        return -1;
    }

    // Conectar ao servidor
    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Falha na conexão");
        return -1;
    }

    // Enviar a mensagem OI para o servidor
    msg_t msg;

    fill_msg(&msg, 0, client_id, 0, "OI");
    if(send_msg(client_fd, &msg)) {
        printf("Mensagem OI enviada para o servidor.\n");
    }

    if(receive_msg(client_fd, &msg)) print_msg(&msg);
    
    // Loop para ler as mensagens do servidor (tipo MSG)
    while (1) {
        if (!receive_msg(client_fd, &msg)) {
            printf("Erro ou desconexão do servidor.\n");
            break;
        }

        // Se a mensagem for do tipo MSG
        if (msg.type == 2) {
            if(msg.orig_uid == 0) {
                printf("\nMensagem do Servidor: %s", msg.text);
            
            } else if (msg.dest_uid == 0) {
                printf("Mensagem de %d: %s\n", msg.orig_uid, msg.text);
            
            } else if (msg.dest_uid == client_id) {
                printf("Mensagem privada de %d: %s\n", msg.orig_uid, msg.text);
            }
        }

        memset(msg.text, '\0', sizeof(msg.text));
    }

    // Fechar o socket
    close(client_fd);

    return 0;
}
