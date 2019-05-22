#include "receive_msg.h"

void *receive_msg(void *ptr) {
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
        printf("%s> ", msg);
        fflush(stdout);
    }
}