#ifndef COMMON_HPP
#define COMMON_HPP

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
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <regex>
#include <time.h>

using namespace std;

extern const char user_protocol[];
extern const char group_protocol[];
extern struct mq_attr attr;
extern mqd_t user_queue;
extern queue<string> fila_msg_enviadas;
extern map<string, bool> users;
extern map<string, bool> joined_groups;
extern sem_t S;
extern map<int, bool> keys;

vector<string> split(const char *str, char c = ':');
bool exists_file(const string &name);
string exec(const char *cmd);
void handle_sigint(int sig);
bool check_key(int key);
int generate_key();

#endif