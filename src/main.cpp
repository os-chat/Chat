#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <string.h>
#include <iostream>

using namespace std;

int main(int argc, char const *argv[]) {
    char queue_name[20];
    strcpy(queue_name, "/chat-");
    strcat(queue_name, argv[1]);
    mqd_t queue;
    struct mq_attr attr;
    char msg[501];

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(msg);
    attr.mq_flags = 0;

    // Abre fila
    if ((queue = mq_open (queue_name, O_RDWR|O_CREAT, 0666, &attr)) < 0) {
        perror ("mq_open");
        exit (1);
    }

    return 0;
}