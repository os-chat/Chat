#include <QApplication>
#include <QWidget>
#include "window.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

// qmake
// make
// ./Chat nickname

typedef void * (*THREADFUNCPTR)(void *);
const char protocolo[] = "/chat-";
struct mq_attr attr;
mqd_t user_queue;

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

void handle_sigint(int sig) {
    printf("\nSe quiser finalizar o programa, digite no campo de mensagens: exit\n");
}

void main_terminal(char *user_name);
int main_interface(int argc, char *argv[], char *user_name);

int main(int argc, char *argv[]) {
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(char)*524;
    attr.mq_flags = 0;
    char user_name[11];
    strcpy(user_name, argv[1]);
    char nome_negado[] = "all";
    int opcao;

    while(!strcmp(user_name, nome_negado)) {
        printf("Nao é permitido usar o nome \'%s\', tente novamente com outro nome: ", user_name);
        scanf(" %[^\n]", user_name);
    }

    printf("-----------------------");
    for(int i = 0; i < strlen(user_name); ++i)
        printf("-");
    printf("\n");
    printf("| Bem-vindo ao Chat, %s |\n", user_name);
    printf("-----------------------");
    for(int i = 0; i < strlen(user_name); ++i)
        printf("-");
    printf("\n\n");

    printf("Como deseja usar o Chat?\n");
    printf("1. Terminal\n");
    printf("2. Interface Gráfica\n");
    printf("0. Sair\n");

    printf("\nOpção: ");
    scanf(" %d", &opcao);

    while(opcao < 0 || opcao > 2) {
        printf("Opção deve ser 0, 1 ou 2, tente novamente: ");
        scanf(" %d", &opcao);
    }

    system("clear");

    if(opcao == 1)
        main_terminal(user_name);
    else if(opcao == 2)
        main_interface(argc, argv, user_name);
    else
        printf("\nEncerrando\n");

    return 0;
}

void main_terminal(char *user_name) {
    signal(SIGINT, handle_sigint);
    char user_queue_name[20];
    strcpy(user_queue_name, protocolo);
    strcat(user_queue_name, user_name);

    if ((user_queue = mq_open(user_queue_name, O_RDWR|O_CREAT, 0622, &attr)) < 0) {
        perror ("mq_open");
        exit (1);
    }

    pthread_t thread_recebe;
    pthread_create(&thread_recebe, NULL, &receber_mensagens, NULL);

    while(1) {
        char destinatario[11];
        char msg_enviada[524];
        char texto[524];
        scanf(" %[^\n]", texto);
        if(strcmp(texto, "exit") == 0)
            break;

        strcpy(msg_enviada, texto);

        char *token;
        token = strtok(texto, ":");
        token = strtok(NULL, ":");
        strcpy(destinatario, token);
        strcat(msg_enviada, "\n");
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
}

int main_interface(int argc, char *argv[], char *user_name) {
    signal(SIGINT, handle_sigint);
    QApplication app(argc, argv);
    Window tela;
    char title[20];

    strcpy(tela.user, user_name);
    strcpy(title, "Chat - ");
    strcat(title, tela.user);

    tela.resize(800, 400);
    tela.setWindowTitle(title);
    char user_queue_name[20];
    strcpy(user_queue_name, protocolo);
    strcat(user_queue_name, tela.user);
    if ((tela.user_queue = mq_open(user_queue_name, O_RDWR|O_CREAT, 0622, &attr)) < 0) {
        perror("mq_open");
        exit(1);
    }
    tela.start();
    tela.show();

    return app.exec();
}
