#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

// g++ main.cpp -lpthread -lrt -o prog
// ./prog nickname

mqd_t user_queue;
const char protocolo[] = "/chat-";
char msg_enviada[524];

void *receber_mensagens(void *ptr) {
    char msg_recebida[524];

    while(1) {
        int i;
        if ((mq_receive (user_queue, msg_recebida, sizeof(msg_recebida), 0)) < 0) {
            perror("mq_receive:");
            exit(1);
        }
        for(i = 0; msg_recebida[i] != ':'; ++i)
            printf("%c", msg_recebida[i]);
        
        printf("%c", msg_recebida[i]);
        i++;
        
        while(msg_recebida[i] != ':')
            i++;

        printf(" ");
        
        while(msg_recebida[++i] != '\0')
            printf("%c", msg_recebida[i]);
    }
}

void enviar_mensagens() {

}

void handle_sigint(int sig) { 
    printf("\nSe quiser finalizar o programa, digite: exit\n"); 
} 

int main(int argc, char const *argv[]) {
    char user_name[11];
    strcpy(user_name, argv[1]);
    char nome_negado[] = "all";

    while(!strcmp(user_name, nome_negado)) {
        printf("Nao é permitido usar o nome \'%s\', tente novamente com outro nome: ", user_name);
        scanf(" %[^\n]", user_name);
    }

    signal(SIGINT, handle_sigint);
    char user_queue_name[20];
    strcpy(user_queue_name, protocolo);
    strcat(user_queue_name, user_name);
    struct mq_attr attr;
    char texto[501];
    char destinatario[11];

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(msg_enviada);
    attr.mq_flags = 0;

    // Abre fila
    // O_RDWR = Open - Read and Write
    if ((user_queue = mq_open(user_queue_name, O_RDWR|O_CREAT, 0622, &attr)) < 0) {
        perror ("mq_open");
        exit (1);
    }

    pthread_t thread_recebe;
    pthread_create(&thread_recebe, NULL, &receber_mensagens, NULL);

    while(1) {
        printf("Mensagem: ");
        scanf(" %[^\n]", texto);
        if(strcmp(texto, "exit") == 0)
            break;

        printf("Destinatario: ");
        scanf(" %[^\n]", destinatario);

        strcpy(msg_enviada, user_name);
        strcat(msg_enviada, ":");
        strcat(msg_enviada, destinatario);
        strcat(msg_enviada, ":");
        strcat(msg_enviada, texto);
        strcat(msg_enviada, "\n");
        // msg = DE:PARA:TEXTO

        mqd_t other_queue;

        char other_queue_name[20];
        strcpy(other_queue_name, protocolo);
        strcat(other_queue_name, destinatario);

        // O_WRONLY = Open - Write Only
        if((other_queue = mq_open (other_queue_name, O_WRONLY)) < 0) {
            printf("UNKNOWNUSER %s\n", destinatario);
        }
        else {
            if (mq_send(other_queue, msg_enviada, sizeof(msg_enviada), 0) < 0) {
                perror ("mq_send");
                exit(1);
            }

            mq_close(other_queue);
        }
    }   

    mq_close(user_queue);

    return 0;
}