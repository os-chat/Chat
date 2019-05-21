#include "common.h"

void handle_sigint(int sig) {
    printf("\nSe quiser finalizar o programa, digite: exit\n");
}

const char protocol[] = "/chat-";
struct mq_attr attr;
mqd_t user_queue;
queue<char*> fila_msg_enviadas;
sem_t S;