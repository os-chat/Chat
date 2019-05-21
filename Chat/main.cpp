#include <QApplication>
#include "window.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <signal.h>
#include <queue>
#include <semaphore.h>
#include <unistd.h>

using namespace std;

typedef void * (*THREADFUNCPTR)(void *);
const char protocolo[] = "/chat-";
struct mq_attr attr;
mqd_t user_queue;
queue<char*> fila_msg_enviadas;
sem_t S;

vector<char*> command_list() {
    vector<char*> users;
    DIR *d;
    struct dirent *dir;
    d = opendir("/dev/mqueue");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            char *token;
            token = strtok(dir->d_name, "-");
            if (strcmp(token, "chat") == 0) {
                token = strtok(NULL, "-");
                users.push_back(token);
            }
        }
        closedir(d);
    }

    return users;
}

void *receber_mensagens(void *ptr) {
    char msg_recebida[524], *user, *dest, msg[524];
    while(1) {
        strcpy(msg, "");
        if ((mq_receive (user_queue, msg_recebida, sizeof(msg_recebida), 0)) < 0) {
            perror("mq_receive:");
            exit(1);
        }
        user = strtok(msg_recebida, ":");
        dest = strtok(NULL, ":");

        if(strcmp(dest, "all") == 0)
            strcpy(msg, "Broadcast de ");

        strcat(msg, user);
        strcat(msg, ": ");
        strcat(msg, strtok(NULL, ":"));
        printf("%s", msg);
    }
}

void *enviar_mensagens(void *ptr) {
    while(1) {
        sem_wait(&S);

        mqd_t other_queue;
        char other_queue_name[20];
        char *user_name, *destinatario, msg_enviada[524], token[524];
        strcpy(token, fila_msg_enviadas.front());
        strcpy(msg_enviada, fila_msg_enviadas.front());

        user_name = strtok(token, ":");
        destinatario = strtok(NULL, ":");

        if(strcmp(destinatario, "all") == 0) { // se o destinatário for all
            vector<char*> users = command_list();
            for(auto u : users) {
                if(strcmp(user_name, u)) {
                    strcpy(other_queue_name, protocolo);
                    strcat(other_queue_name, u);
                    other_queue = mq_open (other_queue_name, O_WRONLY|O_NONBLOCK);
                    if (mq_send(other_queue, msg_enviada, sizeof(msg_enviada), 0) < 0) {
                        perror ("mq_send");
                        exit(1);
                    }

                    mq_close(other_queue);
                }
            }
        }
        else {
            strcpy(other_queue_name, protocolo);
            strcat(other_queue_name, destinatario);

            // O_WRONLY = Open - Write Only
            if((other_queue = mq_open (other_queue_name, O_WRONLY|O_NONBLOCK)) < 0) {
                printf("UNKNOWNUSER %s\n", destinatario);
            }
            else {
                int tentativas = 0;
                while(tentativas <= 3) {
                    if (mq_send(other_queue, msg_enviada, sizeof(msg_enviada), 0) < 0 && errno == EAGAIN) {
                        tentativas++;
                        if(tentativas == 4)
                            break;
                        sleep(5*tentativas);
                    }
                    else
                        break;
                }
                if(tentativas > 3)
                    printf("ERRO %s", msg_enviada);

                mq_close(other_queue);
            }
        }
    }
}

void handle_sigint(int sig) {
    printf("\nSe quiser finalizar o programa, digite: exit\n");
}

void main_terminal(char *user_name);
int main_interface(int argc, char *argv[], char *user_name);

int main(int argc, char *argv[]) {
    sem_init(&S, 0, 0);
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
    for(size_t i = 0; i < strlen(user_name); ++i)
        printf("-");

    printf("\n| Bem-vindo ao Chat, %s |\n", user_name);
    printf("-----------------------");
    for(size_t i = 0; i < strlen(user_name); ++i)
        printf("-");

    printf("\n\nComo deseja usar o Chat?\n");
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
        printf("Encerrando\n");

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

    pthread_t thread_recebe, thread_envia;
    pthread_create(&thread_recebe, NULL, &receber_mensagens, NULL);
    pthread_create(&thread_envia, NULL, &enviar_mensagens, NULL);

    while(1) {
        char user[11], destinatario[11], texto[501];
        char msg_enviada[524];
        strcpy(user, "");
        strcpy(destinatario, "");
        strcpy(texto, "");
        scanf(" %[^:\n]:%[^:]:%500[^\n]", user, destinatario, texto);

        if(!strcmp(user, "exit"))
            break;

        if(!strcmp(user, "list")) {
            printf("\nLista de Usuários:\n");
            vector<char*> users = command_list();
            for(auto u : users)
                printf("%s\n", u);
            continue;
        }

        if(!strlen(destinatario) || !strlen(texto)) {
            printf("Formato inválido, tente novamente.\n");
            continue;
        }

        strcpy(msg_enviada, user);
        strcat(msg_enviada, ":");
        strcat(msg_enviada, destinatario);
        strcat(msg_enviada, ":");
        strcat(msg_enviada, texto);
        strcat(msg_enviada, "\n");

        fila_msg_enviadas.push(msg_enviada);
        sem_post(&S);
    }

    mq_close(user_queue);
    //mq_unlink(user_queue_name);
}

int main_interface(int argc, char *argv[], char *user_name) {
    signal(SIGINT, handle_sigint);
    QApplication app(argc, argv);
    Window tela;
    char title[20];

    strcpy(tela.user, user_name);
    strcpy(title, "Chat - ");
    strcat(title, tela.user);

    tela.resize(800, 550);
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



//   0   5s    1    10s    2   15s    3
