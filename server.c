#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#define PORT 8080

int main(int argc, char **argv, char **argenv)
{
	int server_socket, comm_socket;
	int opt = 1;
	struct sockaddr_in server_address, client_address;
	socklen_t client_size = sizeof(client_address);

	//Criando um 'socket' para o servidor
	if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Falha ao criar o socket do servidor");
		exit(EXIT_FAILURE);
	}

	//Definindo algumas configurações para o 'socket'
	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
			       	&opt, sizeof(opt))) {
		perror("Erro ao configurar o socket do servidor");
		exit(EXIT_FAILURE);
	}
	
	//Atribuindo valores aos atributos do struct de endereço do servidor
	server_address.sin_family = AF_INET; //IPv4
	server_address.sin_port = htons(PORT); //Porta TCP
    server_address.sin_addr.s_addr = INADDR_ANY; //Endereço local

    //Dando um 'bind' do novo 'socket' a um endereço local
	if(bind(server_socket, (const struct sockaddr *) &server_address,
			       	sizeof(server_address)) < 0) {
		perror("Falha ao associar o socket ao endereço local");
		exit(EXIT_FAILURE);
	}

	//Colocando o 'socket' no modo passivo de 'listen'
	if(listen(server_socket, 2) < 0) {
		perror("Falha ao colocar o socket em modo listen");
		exit(EXIT_FAILURE);
	}
       

   	//Aceita uma solicitação de conexão com um cliente
	if((comm_socket = accept(server_socket, (struct sockaddr *) &client_address,
								   	&client_size)) < 0) {
			perror("Falha ao aceitar a conexão de um cliente");
			exit(EXIT_FAILURE);
	}
	
	return(0);
} 
