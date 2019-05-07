#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

// g++ main.cpp -lpthread -lrt -o prog
// ./prog nickname

void receber_mensagens() {

}

void enviar_mensagens() {

}

void handle_sigint(int sig) { 
    printf("\nSe quiser finalizar o programa, digite \"exit\"\n"); 
} 

int main(int argc, char const *argv[]) {
    signal(SIGINT, handle_sigint);
    char queue_name[20];
    strcpy(queue_name, "/chat-");
    strcat(queue_name, argv[1]);
    mqd_t queue;
    struct mq_attr attr;
    char msg[524];
    char texto[501];
    char destinatario[11];

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(msg);
    attr.mq_flags = 0;

    // Abre fila
    // O_RDWR = Open - Read and Write
    if ((queue = mq_open (queue_name, O_RDWR|O_CREAT, 0666, &attr)) < 0) {
        perror ("mq_open");
        exit (1);
    }

    while(1) {
        printf("Mensagem: ");
        scanf(" %[^\n]", texto);
        if(strcmp(texto, "exit") == 0)
            break;

        printf("Destinatario: ");
        scanf(" %[^\n]", destinatario);
    }   

    mq_unlink(queue_name);

    return 0;
}