#include <time.h>
#include <sys/select.h>

#define TIME 1000
#define MAX_CLIENTS 999

time_t time_flag;
time_t time_flag2;
time_t time_start_server;

fd_set fds_current;

unsigned short int active_messager_ids[MAX_CLIENTS]; // Array para armazenar os IDs de clientes de envio
unsigned short int active_display_ids[MAX_CLIENTS]; // Array para armazenar os IDs de clientes de exibição