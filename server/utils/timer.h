#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>

/**
 * Envio de mensagem do servidor a cada 60 segundos informandos os clientes de exibicao,
 * quantos clientes ha conectados e quanto tempo o servidor tem de execucao
 */
void message_servidor() {
	msg_t message;
	int cont = 0;

    // Contagem de clientes conectados
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if(active_display_ids[i] != 0) {
			cont++;
		}
	}

    // Montagem de mensagem
	char text[150];
    sprintf(text, "Servidor de Redes\n- Clientes de exibicao conectados: %d\n- Tempo de execucao do servidor: %.d minutos,\n\n", cont, time_flag2/60000);

    // Envio de mensagem ao clientes
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if(active_display_ids[i] != 0) {
            fill_msg(&message, 2, 0, i + 1, text);
            send_msg(active_display_ids[i], &message);
        }
    }
}

/**
 Reinicia o timer
 */
void set_timer(int miliseconds) {
    struct itimerval newvalue, oldvalue;

    newvalue.it_value.tv_sec  = miliseconds / 1000;
    newvalue.it_value.tv_usec = miliseconds % 1000 * 1000;
    newvalue.it_interval.tv_sec  = 0;
    newvalue.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &newvalue, &oldvalue);
}

void timer_handler(int signum) {
    // Atualizacao de tempos flag
    time_flag ++;
    time_flag2 += TIME;

    // Envia uma mensagem do servidor a todos os clientes de exibicao a cada 60 segundos
    if(time_flag >= 60) {
        message_servidor();
        time_flag = 0;
    }

    // Reinicia o timer
    set_timer(TIME);
}

/*
 Configura um sinal para o programa alterar seu fluxo apos uma entrada
*/
void set_handler(void) {
    signal(SIGALRM, timer_handler);
}

/*
    Configura e inicializa o temporizador
*/
void start_timer() {
    size_t elen;

    set_handler();
    set_timer(TIME);

    // Iniciando tempo do servidor
    time_flag = 0;
	time_flag2 = 0;
}