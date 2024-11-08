#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>  // Incluir a biblioteca que contém a declaração de atoi
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "msg_utils.h"

int main(int argc, char const *argv[])
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};  // Buffer para armazenar a mensagem recebida do servidor

    // Verificar se foi fornecido um identificador de cliente (argumento da linha de comando)
    if (argc != 2) {
        printf("Uso: %s <ID do Cliente (1-999)>\n", argv[0]);
        return -1;
    }

    // Usar atoi para converter o ID do cliente (agora funciona corretamente)
    int client_id = atoi(argv[1]);

    // Verificar se o ID é válido (1-999)
    if (client_id < 1 || client_id > 999) {
        printf("ID do cliente inválido. O ID deve ser entre 1 e 999.\n");
        return -1;
    }

    // Mostrar o ID do cliente
    printf("Cliente ID: %d\n", client_id);

    // Criar o socket TCP
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erro ao criar o socket");
        return -1;
    }

    // Configurar as informações do servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);  // A porta do servidor (8080)

    // Converter o endereço IP do servidor (aqui, usaremos "127.0.0.1" para localhost)
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Endereço inválido ou não suportado");
        return -1;
    }

    // Conectar ao servidor
    if ((status = connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0) {
        perror("Falha na conexão");
        return -1;
    }

    printf("Conectado ao servidor.\n");

    // Enviar a mensagem OI para o servidor
    msg_t oi_message;
    oi_message.type = htons(0);  // Tipo OI
    oi_message.orig_uid = htons(client_id);  // Identificador do cliente
    oi_message.dest_uid = htons(0);  // Destinatário 0, já que a mensagem é apenas para identificar
    oi_message.text_len = htons(0);  // Não há texto na mensagem OI
    memset(oi_message.text, 0, MAX_TEXT_LEN + 1); // Garantir que o campo de texto esteja vazio

    if (send(client_fd, &oi_message, sizeof(oi_message), 0) == -1) {
        perror("Erro ao enviar OI");
        close(client_fd);
        return -1;
    }

    printf("Mensagem OI enviada para o servidor.\n");

    // Ler a resposta do servidor (deve ser uma mensagem OI de volta)
    valread = read(client_fd, buffer, 1024 - 1);  // Lê até 1023 caracteres
    if (valread > 0) {
        buffer[valread] = '\0';  // Garantir que a string esteja terminada em '\0'
        printf("Mensagem recebida do servidor:\n%s\n", buffer);  // Exibe a mensagem recebida
    } else {
        printf("Falha ao ler a mensagem do servidor.\n");
        close(client_fd);
        return -1;
    }

    // Loop para ler as mensagens do servidor (tipo MSG)
    while (1) {
        valread = read(client_fd, &buffer, sizeof(buffer));
        if (valread > 0) {
            msg_t *received_msg = (msg_t *)buffer;
            received_msg->type = ntohs(received_msg->type);
            received_msg->orig_uid = ntohs(received_msg->orig_uid);
            received_msg->dest_uid = ntohs(received_msg->dest_uid);
            received_msg->text_len = ntohs(received_msg->text_len);

            if (received_msg->type == 2) {  // Se a mensagem for do tipo MSG
                if (received_msg->dest_uid == 0) {
                    printf("Mensagem de %d (enviada para todos): %s\n",
                           received_msg->orig_uid, received_msg->text);
                } else if (received_msg->dest_uid == client_id) {
                    printf("Mensagem privada de %d: %s\n", received_msg->orig_uid, received_msg->text);
                }
            }
        } else {
            printf("Erro ou desconexão do servidor.\n");
            break;
        }
    }

    // Fechar o socket
    close(client_fd);

    return 0;
}
