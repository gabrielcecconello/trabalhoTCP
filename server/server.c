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
#include "../utils/msg_utils.h"

#define MAX_CLIENTS 100

int active_client_ids[MAX_CLIENTS]; // Array para armazenar os IDs de clientes

int num_clients = 0; // Contador de clientes conectados

// Função para configurar o servidor (sem alterações)
int setting_server()
{
	memset(active_client_ids, -1, sizeof(active_client_ids));
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

// Função para verificar se o identificador do cliente está em uso
int is_client_id_used(int client_id)
{
	// Verifica se o ID do cliente ou o ID + 1000 já está em uso
	for (int i = 0; i < num_clients; i++)
	{
		if (active_client_ids[i] == client_id || active_client_ids[i] == client_id + 1000)
		{
			return 1; // ID já em uso
		}
	}
	return 0; // ID disponível
}

// Função para lidar com a conexão
int handle_connection(int comm_socket)
{
	char *hello = "Hello from server\n";
	msg_t msg;
	initialize_msg(&msg);
	unsigned short int msg_type;
	int client_id;

	// Lê a mensagem e exibe
	if (!receive_msg(comm_socket, &msg))
		return 1;

	msg_type = msg.type;
	print_msg(&msg);

	// Verifica se a mensagem contém um ID (exemplo: OI com ID)
	if (msg_type == 0 || msg_type == 2)
	{

		client_id = msg.orig_uid; // Assumimos que msg.id está vindo corretamente da mensagem do cliente

		// Verifica se o ID está em uso
		if (is_client_id_used(client_id))
		{
			// Envia mensagem de erro para o cliente
			char *error_msg = "Erro: Identificador já está em uso.\n";
			fill_msg(&msg, 1, 0, client_id, error_msg);
			send_msg(comm_socket, &msg);
			printf("Houve uma tentativa de conexão com o client de ID %d, mas esse identificador já se encontra em uso!\n\n\n", client_id);
			// close(comm_socket); // Fecha a conexão
			return 1; // Indica erro
		}
		else
		{
			// Registra o ID do cliente
			if (num_clients < MAX_CLIENTS)
			{
				active_client_ids[num_clients++] = client_id;
				for (int i = 0; i < MAX_CLIENTS; i++)
				{
					printf("%d ", active_client_ids[i]);
				}
				printf("\n");

				// Armazena o ID e incrementa o contador
				printf("Cliente com ID %d registrado.\n\n\n", client_id);
			}
			else
			{
				// Se não houver espaço para mais clientes
				char *full_msg = "Erro: Número máximo de clientes alcançado.\n";
				fill_msg(&msg, 1, 0, client_id, full_msg);
				send_msg(comm_socket, &msg);
				// close(comm_socket); // Fecha a conexão
				return 1;
			}
		}
	}

	// Se não houver problemas com o ID, envia uma mensagem de resposta
	fill_msg(&msg, 2, 0, client_id, hello);
	send_msg(comm_socket, &msg);

	return msg_type;
}

// Função principal
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

	while (1)
	{
		// Sempre realimentando o set temporário de FDs com o set definitivo
		fds_ready = fds_current;

		// Reiniciando o timeout a cada iteração
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		if (select(highest_fd + 1, &fds_ready, NULL, NULL, &timeout) < 0)
		{
			perror("Falha na operação de 'select'");
			exit(EXIT_FAILURE);
		}

		for (int i = server_socket; i <= highest_fd; i++)
		{
			if (FD_ISSET(i, &fds_ready))
			{
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
	}

	return (0);
}
