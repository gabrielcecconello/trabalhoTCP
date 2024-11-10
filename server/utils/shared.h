#define TIME 1000
#define MAX_CLIENTS 999

int time_flag;
int time_flag2;

unsigned short int active_messager_ids[MAX_CLIENTS]; // Array para armazenar os IDs de clientes de envio
unsigned short int active_display_ids[MAX_CLIENTS]; // Array para armazenar os IDs de clientes de exibição