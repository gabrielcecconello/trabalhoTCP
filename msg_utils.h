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

    send(socket, buffer, sizeof(buffer), 0);
}

void receive_msg(int socket, msg_t* msg) {
    unsigned char buffer[BUFFER_SIZE];

    ssize_t bytes_received = read(socket, buffer, sizeof(buffer));
    if(bytes_received > 0) {
        deserialize_msg(msg, buffer);
    }
}

void print_buffer_as_bytes(unsigned char* buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", buffer[i]);  // Imprime o byte em formato hexadecimal
    }
    printf("\n");
}