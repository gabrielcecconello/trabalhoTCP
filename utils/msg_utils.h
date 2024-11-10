#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 149

typedef struct
{
	unsigned short int type;
	unsigned short int orig_uid;
	unsigned short int dest_uid;
	unsigned short int text_len;
	unsigned short int id;
	unsigned char text[141];
} msg_t;

void initialize_msg(msg_t *msg)
{
	msg->type = 0;
	msg->orig_uid = 0;
	msg->dest_uid = 0;
	strncpy(msg->text, "", sizeof(msg->text) - 1);
	msg->text[sizeof(msg->text) - 1] = '\0';
	msg->text_len = strlen(msg->text);
}

// void fill_msg(
// 	msg_t *msg,
// 	unsigned short int type,
// 	unsigned short int orig_uid,
// 	unsigned short int dest_uid,
// 	unsigned char *text,
// 	unsigned short int id)
// {
// 	msg->type = type;
// 	msg->orig_uid = orig_uid;
// 	msg->dest_uid = dest_uid;

// 	strncpy(msg->text, text, sizeof(msg->text) - 1);
// 	msg->text[sizeof(msg->text) - 1] = '\0';

// 	//msg->text_len = strlen(msg->text);
// 	msg->text_len = strlen((char *)msg->text);
// }
//==========================================================================
// void fill_msg(
//     msg_t *msg,
//     unsigned short int type,
//     unsigned short int orig_uid,
//     unsigned short int dest_uid,
//     unsigned char *text,
//     unsigned short int id)
// {
//     // Atribuindo os valores aos campos da estrutura
//     msg->type = type;
//     msg->orig_uid = orig_uid;
//     msg->dest_uid = dest_uid;

//     // Copiando o texto para o campo 'text', garantindo que não ultrapasse 140 caracteres
//     // 'sizeof(msg->text) - 1' garante que o último espaço será reservado para '\0'
//     strncpy((char *)msg->text, (char *)text, sizeof(msg->text) - 1);

//     // Garantindo que a string termine com '\0' após o strncpy
//     msg->text[sizeof(msg->text) - 1] = '\0';

//     // Calculando o comprimento da string copiada para o campo 'text'
//     msg->text_len = strlen((char *)msg->text);
// }
void fill_msg(
	msg_t *msg,
	unsigned short int type,
	unsigned short int orig_uid,
	unsigned short int dest_uid,
	unsigned char *text)
{
	// Atribuindo valores aos campos da estrutura
	msg->type = type;
	msg->orig_uid = orig_uid;
	msg->dest_uid = dest_uid;

	// Calculando o comprimento do texto
	size_t text_len = strlen((char *)text);

	// Garantir que o texto copiado não ultrapasse 140 caracteres
	if (text_len > sizeof(msg->text) - 1)
	{
		text_len = sizeof(msg->text) - 1; // Limitar ao máximo possível
	}

	// Copiando o texto para o campo 'text', garantindo que não ultrapasse o limite de 140 caracteres
	strncpy((char *)msg->text, (char *)text, text_len);

	// Garantindo que a string copiada termine com '\0'
	msg->text[text_len] = '\0';

	// Atribuindo o comprimento da mensagem
	msg->text_len = text_len;
}

void deserialize_msg(msg_t *msg, unsigned char *buffer)
{
	int offset = 0;

	unsigned short int type;
	memcpy(&type, buffer + offset, sizeof(type));
	msg->type = ntohs(type);
	offset += sizeof(type);

	unsigned short int orig_uid;
	memcpy(&orig_uid, buffer + offset, sizeof(orig_uid));
	msg->orig_uid = ntohs(orig_uid);
	offset += sizeof(orig_uid);

	unsigned short int dest_uid;
	memcpy(&dest_uid, buffer + offset, sizeof(dest_uid));
	msg->dest_uid = ntohs(dest_uid);
	offset += sizeof(dest_uid);

	unsigned short int text_len;
	memcpy(&text_len, buffer + offset, sizeof(text_len));
	msg->text_len = ntohs(text_len);
	offset += sizeof(text_len);

	memcpy(msg->text, buffer + offset, msg->text_len);
}
// void deserialize_msg(msg_t *msg, unsigned char *buffer)
// {
// 	int offset = 0;

// 	unsigned short int type;
// 	memcpy(&type, buffer + offset, sizeof(type));
// 	msg->type = ntohs(type); // Convertendo de rede para host
// 	offset += sizeof(type);

// 	unsigned short int orig_uid;
// 	memcpy(&orig_uid, buffer + offset, sizeof(orig_uid));
// 	msg->orig_uid = ntohs(orig_uid); // Convertendo de rede para host
// 	offset += sizeof(orig_uid);

// 	unsigned short int dest_uid;
// 	memcpy(&dest_uid, buffer + offset, sizeof(dest_uid));
// 	msg->dest_uid = ntohs(dest_uid); // Convertendo de rede para host
// 	offset += sizeof(dest_uid);

// 	unsigned short int text_len;
// 	memcpy(&text_len, buffer + offset, sizeof(text_len));
// 	msg->text_len = ntohs(text_len); // Convertendo de rede para host
// 	offset += sizeof(text_len);

// 	// Verificar se text_len é maior que o tamanho do campo text
// 	if (msg->text_len > sizeof(msg->text) - 1)
// 	{
// 		msg->text_len = sizeof(msg->text) - 1; // Garantir que não ultrapasse o limite
// 	}

// 	memcpy(msg->text, buffer + offset, msg->text_len);
// 	msg->text[msg->text_len] = '\0'; // Garantir que o texto seja uma string válida
// }

// void serialize_msg(msg_t *msg, unsigned char *buffer)
// {
// 	int offset = 0;

// 	unsigned short int type = htons(msg->type);
// 	memcpy(buffer + offset, &type, sizeof(type));
// 	offset += sizeof(type);

// 	unsigned short int orig_uid = htons(msg->orig_uid);
// 	memcpy(buffer + offset, &orig_uid, sizeof(orig_uid));
// 	offset += sizeof(orig_uid);

// 	unsigned short int dest_uid = htons(msg->dest_uid);
// 	memcpy(buffer + offset, &dest_uid, sizeof(dest_uid));
// 	offset += sizeof(dest_uid);

// 	unsigned short int text_len = htons(msg->text_len);

// 	// Verificar se o texto não ultrapassa o tamanho máximo
// 	if (msg->text_len > sizeof(msg->text) - 1)
// 	{
// 		text_len = htons(sizeof(msg->text) - 1); // Ajustar o comprimento do texto se necessário
// 	}

// 	memcpy(buffer + offset, &text_len, sizeof(text_len));
// 	offset += sizeof(text_len);

// 	// Copiar o texto para o buffer, garantindo que não ultrapasse o limite
// 	memcpy(buffer + offset, msg->text, ntohs(text_len));
// }

void serialize_msg(msg_t *msg, unsigned char *buffer)
{
	int offset = 0;

	unsigned short int type = htons(msg->type);
	memcpy(buffer + offset, &type, sizeof(type));
	offset += sizeof(type);

	unsigned short int orig_uid = htons(msg->orig_uid);
	memcpy(buffer + offset, &orig_uid, sizeof(orig_uid));
	offset += sizeof(orig_uid);

	unsigned short int dest_uid = htons(msg->dest_uid);
	memcpy(buffer + offset, &dest_uid, sizeof(dest_uid));
	offset += sizeof(dest_uid);

	unsigned short int text_len = htons(msg->text_len);
	memcpy(buffer + offset, &text_len, sizeof(text_len));
	offset += sizeof(text_len);

	memcpy(buffer + offset, msg->text, msg->text_len);
}

int send_msg(int socket, msg_t *msg)
{
	unsigned char buffer[BUFFER_SIZE];
	serialize_msg(msg, buffer);

	if (send(socket, buffer, BUFFER_SIZE - 1, 0) < 0)
	{
		perror("Falha ao enviar a mensagem.\n\n");
		return 0;
	}

	memset(&buffer, 0, BUFFER_SIZE);
	return 1;
}

// void receive_msg(int socket, msg_t *msg)
// {
// 	unsigned char buffer[BUFFER_SIZE];

// 	// Tenta ler do socket
// 	ssize_t bytes_received = read(socket, buffer, BUFFER_SIZE - 1);

// 	if (bytes_received > 0)
// 	{
// 		printf("Mensagem Recebida!\n");
// 		deserialize_msg(msg, buffer); // Desserializa a mensagem
// 	}
// 	else if (bytes_received == 0)
// 	{
// 		// Conexão fechada graciosamente pelo peer
// 		printf("Conexão fechada pelo cliente.\n");
// 	}
// 	else if (bytes_received < 0)
// 	{
// 		// Falha ao ler a mensagem
// 		perror("Falha ao ler mensagem");
// 	}

// 	// Limpeza do buffer (não é estritamente necessário)
// 	memset(buffer, 0, BUFFER_SIZE);
// }

int receive_msg(int socket, msg_t *msg)
{
	unsigned char buffer[BUFFER_SIZE];

	ssize_t bytes_received = read(socket, buffer, BUFFER_SIZE - 1);
	if (bytes_received > 0)
	{
		deserialize_msg(msg, buffer);
	}
	else if (bytes_received <= 0)
	{
		perror("Falha ao ler mensagem.\n\n");
		return 0;
	}

	fflush(stdout);
	memset(&buffer, 0, BUFFER_SIZE);
	return 1;
}

void print_buffer_as_bytes(unsigned char *buffer, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		printf("%02X ", buffer[i]); // Imprime o byte em formato hexadecimal
	}
	printf("\n\n");
	fflush(stdout);
}

void print_msg(msg_t *msg)
{
	printf("Tipo: %d\nOrigem: %d\nDestino: %d\nTamanho da Mensagem: %d\nMensagem: %s\n\n",
		   msg->type,
		   msg->orig_uid,
		   msg->dest_uid,
		   msg->text_len,
		   msg->text);
	fflush(stdout);
}