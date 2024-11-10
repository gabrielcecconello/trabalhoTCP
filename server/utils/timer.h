#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <signal.h>

#define TIME 1000

/**
 Reinicia o time
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
    printf("parouu");
    /* Uma outra opcao seria setar uma variavel global
     * e testa-la no loop principal do programa, mas isso
     * so funcionaria nos casos onde a chamada bloqueada 
     * fosse interrompida pelo sinal
     * (o que nao acontece no caso do teclado, pelo menos).
     */
    set_timer(TIME);  /* Melhor lugar para reiniciar o timer */
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
}

// TODO: avaliar a necessidade dessa funcao
void update_timer() {
    char input[512];
    bool done = false;
    
    fprintf(stderr,"Escreva algo:\n");

    // Leitura da entrada colocada
    if (fgets(input, sizeof(input), stdin) != NULL) {
        done = true;
        //break;
    }
    
    errno = 0; /* So por garantia */
    
    /* fgets retorna erro se a input estava vazia ou se deu erro */
    /* chamadas de socket so retornam < 0 se deu erro */
    if (errno == EINTR) {
        /* uma chamada interrompida seria tratada aqui */
        errno = 0;
    } else if (errno) {
        perror("fgets");
        exit(1);
    } else if (feof(stdin)) {
        fprintf(stderr,"entrada vazia.\n");
        exit(0);
    }
}