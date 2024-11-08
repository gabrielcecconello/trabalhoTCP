#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>      

#define PORT 8080
#define BUFFER_SIZE 149

typedef struct {
    unsigned short int type;
    unsigned short int orig_uid;
    unsigned short int dest_uid;
    unsigned short int text_len;
    unsigned char text[141];
} msg_t;

void initialize_msg(msg_t* msg) {
	msg->type = 0;
	msg->orig_uid = 0;
	msg->dest_uid = 0;
	strncpy(msg->text, "", sizeof(msg->text) - 1);
	msg->text[sizeof(msg->text) - 1] = '\0';
	msg->text_len = strlen(msg->text);
}
 
void fill_msg(
	msg_t* msg,
	unsigned short int type,
	unsigned short int orig_uid,
	unsigned short int dest_uid,
	unsigned char* text
) {
	msg->type = type;
	msg->orig_uid = orig_uid;
	msg->dest_uid = dest_uid;

	strncpy(msg->text, text, sizeof(msg->text) - 1);
	msg->text[sizeof(msg->text) - 1] = '\0';

	msg->text_len = strlen(msg->text);
}

void deserialize_msg(msg_t* msg, unsigned char* buffer) {
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

void serialize_msg(msg_t* msg, unsigned char* buffer) {
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

void send_msg(int socket, msg_t* msg) {
    unsigned char buffer[BUFFER_SIZE];
    serialize_msg(msg, buffer);

    if(send(socket, buffer, BUFFER_SIZE - 1, 0) < 0) {
		printf("Falha ao enviar a mensagem.\n");
	}

	memset(&buffer, 0, BUFFER_SIZE);
}

void receive_msg(int socket, msg_t* msg) {
    unsigned char buffer[BUFFER_SIZE];

    ssize_t bytes_received = read(socket, buffer, BUFFER_SIZE - 1);
    if(bytes_received > 0) {
		printf("Mensagem Recebida!\n");
        deserialize_msg(msg, buffer);
    }
	else if(bytes_received < 0) {
		printf("Falha ao ler mensagem.\n");
	}

	fflush(stdout);
	memset(&buffer, 0, BUFFER_SIZE);
}

void print_buffer_as_bytes(unsigned char* buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", buffer[i]);  // Imprime o byte em formato hexadecimal
    }
    printf("\n\n");
	fflush(stdout);
}

void print_msg(msg_t* msg) {
	printf("Tipo: %d\nOrigem: %d\nDestino: %d\nTamanho da Mensagem: %d\nMensagem: %s\n\n",
	 		msg->type,
	 		msg->orig_uid,
	  		msg->dest_uid,
	  		msg->text_len,
	  		msg->text
	);
	fflush(stdout);
}