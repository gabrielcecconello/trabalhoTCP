#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char *hello = "COME MEU CUZINHO LUIZOLAS E======D";
    char buffer[1024] = {0};
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

    if ((status = connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
    {
        printf("\n Falha na conexão \n");
        return -1;
    }

    send(client_fd, hello, strlen(hello), 0);
    printf("Hello menssage sent\n");
    valread = read(client_fd, buffer, 1024 - 1);
    printf("%s\n", buffer);

    close(client_fd);
    return 0;
}