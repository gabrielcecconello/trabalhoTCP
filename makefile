# Como usar:
# make  # Compila e gera todos os binários
# make clear  # Remove todos os binários

# Especifica o compilador a ser utilizado
CC := gcc

all: final

final: server_exec client_exec displayClient_exec
	@echo "Compilando e gerando os bínarios..."

server_exec: ./server/server.c
	$(CC) ./server/server.c -o server_exec

client_exec: ./client/client.c
	$(CC) ./client/client.c -o client_exec

displayClient_exec: ./client/displayClient.c
	$(CC) ./client/displayClient.c -o displayClient_exec

clear:
	@echo "Limpando todos os binários..."
	rm -v server_exec client_exec displayClient_exec