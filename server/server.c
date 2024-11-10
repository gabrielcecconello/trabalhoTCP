#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <time.h>

#include "../utils/msg_utils.h"
#include "./utils/shared.h"
#include "./utils/timer.h"

// Função para configurar o servidor (sem alterações)
int setting_server()
{
	int opt = 1;
	int server_socket;
	struct sockaddr_in server_address;

	// Criando um 'socket' para o servidor
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Falha ao criar o socket do servidor");
		exit(EXIT_FAILURE);
	}

	// Definindo algumas configurações para o 'socket'
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("Erro ao configurar o socket do servidor");
		exit(EXIT_FAILURE);
	}

	// Atribuindo valores aos atributos do struct de endereço do servidor
	server_address.sin_family = AF_INET;		 // IPv4
	server_address.sin_port = htons(PORT);		 // Porta TCP
	server_address.sin_addr.s_addr = INADDR_ANY; // Endereço local

	// Dando um 'bind' do novo 'socket' a um endereço local
	if (bind(server_socket, (const struct sockaddr *)&server_address, sizeof(server_address)) < 0)
	{
		perror("Falha ao associar o socket ao endereço local");
		exit(EXIT_FAILURE);
	}

	// Colocando o 'socket' no modo passivo de 'listen'
	if (listen(server_socket, 2) < 0)
	{
		perror("Falha ao colocar o socket em modo listen");
		exit(EXIT_FAILURE);
	}

	return server_socket;
}

// Função para aceitar uma nova conexão de cliente (sem alterações)
int accept_new_connection(int server_socket)
{
	int comm_socket;
	struct sockaddr_in client_address;
	socklen_t client_size = sizeof(client_address);

	// Aceita uma solicitação de conexão com um cliente
	comm_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_size);
	if (comm_socket < 0)
	{
		perror("Falha ao aceitar a conexão de um cliente");
		exit(EXIT_FAILURE);
	}

	return comm_socket;
}

// Verifica se o identificador do cliente é de exibição ou de envio
int verify_messager(int client_id) {
	if(1 <= client_id && client_id <= 999) return 0;
	else if(1001 <= client_id && client_id <= 1999) return 1;
}

// Função para verificar se o identificador do cliente está em uso
int is_client_id_used(int client_id)
{
	if(!verify_messager(client_id)) {
		if(active_display_ids[client_id-1] == 0) return 0;
		else return 1;
	}
	else {
		if(active_messager_ids[client_id-1] == 0) return 0;
		else return 1;
	}
}

// OLHAR SE TEM COMO MELHORAR ESSE TREM FEIO
int get_id(int comm_socket) {
	for(int i = 0; i < MAX_CLIENTS; i++) {
		if(active_display_ids[i] == comm_socket) return i+1;
		else if(active_messager_ids[i] == comm_socket) return i+1001;
	}
	return 0;
}

// Função para lidar com a conexão
int handle_connection(int comm_socket)
{
	msg_t msg;
	initialize_msg(&msg);
	unsigned short int msg_type, orig_uid, dest_uid, is_messager;

	// Lê a mensagem e exibe
	if (!receive_msg(comm_socket, &msg)) {
		msg_type = 1;
		printf("Erro na leitura. Cliente desconectado.\n\n");
		orig_uid = get_id(comm_socket);
	}	
	else {
		msg_type = msg.type;
		orig_uid = msg.orig_uid;
		dest_uid = msg.dest_uid;
		print_msg(&msg);
	}

	is_messager = verify_messager(orig_uid);
	
	switch(msg_type) {
		case 0:
			// Verifica se o ID está em uso
			if (is_client_id_used(orig_uid))
			{
				// Envia mensagem de erro para o cliente
				char *error_msg = "Erro - Identificador já está em uso.";
				fill_msg(&msg, 1, 0, orig_uid, error_msg);
				send_msg(comm_socket, &msg);
				printf("Houve uma tentativa de conexão como um cliente com ID %d, mas esse identificador já se encontra em uso!\n\n", orig_uid);
				return 1; // Indica erro
			}
			// Registra o ID do cliente
			if(!is_messager) active_display_ids[orig_uid-1] = comm_socket;
			else active_messager_ids[orig_uid-1001] = comm_socket;

			// Reenvio da mensagem de OI vinda do cliente.
			fill_msg(&msg, 0, 0, orig_uid, msg.text);
			send_msg(comm_socket, &msg);

			// Armazena o ID e incrementa o contador
			printf("Mensagem OI enviada para cliente com ID %d registrado.\n\n", orig_uid);
			break;

		case 1:
			if(!is_messager) active_display_ids[orig_uid-1] = 0;
			else active_messager_ids[orig_uid-1001] = 0;
			break;

		case 2:
			// Se o destino for todos os exibidores
			if(dest_uid == 0) {
				// Itera por todos os exibidores cadastrados
				for(int i = 0; i < MAX_CLIENTS; i++) {
					if(active_display_ids[i] != 0) {
						send_msg(active_display_ids[i], &msg);
					}
				}
			
			} else if(active_display_ids[dest_uid-1] != 0) {
				send_msg(active_display_ids[dest_uid-1], &msg);
			}
			break;
	}

	return msg_type;
}

int main(int argc, char **argv, char **argenv)
{
	int server_socket, comm_socket;
	fd_set fds_ready, fds_current;
	int highest_fd;
	struct timeval timeout;
	unsigned short int msg_type;

	// Abrindo o socket de comunicação do servidor
	server_socket = setting_server();

	// Colocando o servidor como o maior FD
	highest_fd = server_socket;

	// Inicializando o set definitivo de FDs
	FD_ZERO(&fds_current);
	FD_SET(server_socket, &fds_current);

	// Inciando temporizador
	start_timer();

	system("clear");
	printf("\t\t\t*** Servidor ***\n\n");
	
	while (1)
	{
		errno = 0;

		// Sempre realimentando o set temporário de FDs com o set definitivo
		fds_ready = fds_current;

		// Reiniciando o timeout a cada iteração
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

 		int select_result = select(highest_fd + 1, &fds_ready, NULL, NULL, &timeout);

		if (select_result < 0)
		{
			if(errno == EAGAIN) {
				set_handler();
				set_timer(TIME);
				continue;
			
			} else if (errno == EINTR) {
				set_handler();
				set_timer(TIME);
				errno = 0;
				continue;

			} else if (errno) {
				perror("select");
				exit(1);
			}
		}	else if (select_result == 0) {
			// Timeout ocorreu, mas sem dados prontos para leitura, reiniciamos o loop
			continue;
		}
			
		for (int i = server_socket; i <= highest_fd; i++) {
			
			if (!FD_ISSET(i, &fds_ready)) continue;

			if (i == server_socket)
			{
				comm_socket = accept_new_connection(server_socket);
				FD_SET(comm_socket, &fds_current);
				if (comm_socket > highest_fd)
				{
					highest_fd = comm_socket;
				}
			}
			else
			{
				msg_type = handle_connection(i);
				// Caso a mensagem seja de 'tchau' (ou outro tipo de desconexão)
				if (msg_type == 1)
				{
					FD_CLR(i, &fds_current);
					close(i);
				}
			}
		}
	}

	return (0);
}
