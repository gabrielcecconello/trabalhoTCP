#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#define PORT 8080

typedef struct {
    unsigned short int type;
    unsigned short int orig_uid;
    unsigned short int dest_uid;
    unsigned short int text_len;
    unsigned char text[141];
} msg_t;

int setting_server() 
{
	int opt = 1;
	int server_socket;
	struct sockaddr_in server_address;

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

	return server_socket;
}

int accept_new_connection(int server_socket)
{
	int comm_socket;
	struct sockaddr_in client_address;
	socklen_t client_size = sizeof(client_address);

	//Aceita uma solicitação de conexão com um cliente
	comm_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_size);
	if(comm_socket < 0) {
		perror("Falha ao aceitar a conexão de um cliente");
		exit(EXIT_FAILURE);
	}

	return comm_socket;
}

msg_t divide_buffer(char[149] buffer) {
	msg_t msg;
	//TODO: fazer a lógica que divide o buffer na mensagem formatada
}

//TODO: resolver a lógica de leitura e envio da mensagem
void handle_connection(int comm_socket) {
	char buffer[149] = { 0 };
	msg_t formatted_msg;
	ssize_t msg_byte_num;
	char* hello = "Hello from server\n";

	//Lê a mensagem e guarda o número de bytes lidos
	msg_byte_num = read(comm_socket, buffer, 148);
	printf("%s", buffer);

	formatted_msg = divide_buffer(buffer);

	//Esvazia o buffer após exibir a mensagem
	memset(&buffer, 0, msg_byte_num); 

	//Manda uma mensagem padrão para o cliente
	send(comm_socket, hello, strlen(hello), 0);
	printf("'Hello' message sent\n");
}

int main(int argc, char **argv, char **argenv)
{
	int server_socket, comm_socket;
	fd_set fds_ready, fds_current;
	int highest_fd;
	struct timeval timeout;


	//Abrindo o socket de comunicação do servidor
	server_socket = setting_server();
	
	//Colocando o servidor como o maior FD
	highest_fd = server_socket;

	//Inicializando o set definitivo de FDs
	FD_ZERO(&fds_current);
	FD_SET(server_socket, &fds_current);

	while(1) {
		//Sempre realimentando o set temporário de FDs com o set definitivo
		fds_ready = fds_current;

		//Reiniciando o timeout a cada iteração
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		if(select(highest_fd + 1, &fds_ready, NULL, NULL, &timeout) < 0) {
			perror("Falha na operação de 'select'");
			exit(EXIT_FAILURE);
		}

		for(int i = server_socket; i <= highest_fd; i++) {
			if(FD_ISSET(i, &fds_ready)) {
				if(i == server_socket) {
					comm_socket = accept_new_connection(server_socket);
					FD_SET(comm_socket, &fds_current);
					if(comm_socket > highest_fd)  {
						highest_fd = comm_socket;
					}
				}
				else {
					handle_connection(i);
					//FD_CLR(i, &fds_current);
				}
			}
		}	
	}
	
	return(0);
} 
