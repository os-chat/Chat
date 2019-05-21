#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>
#include <errno.h>
#include <mqueue.h>
#include <dirent.h>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <signal.h>
#include <queue>
#include <semaphore.h>
#include <string.h>

using namespace std;

extern const char protocol[];
extern struct mq_attr attr;
extern mqd_t user_queue;
extern queue<char*> fila_msg_enviadas;
extern sem_t S;

void handle_sigint(int sig);

#endif