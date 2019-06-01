#include "common.h"

void handle_sigint(int sig)
{
    printf("\nSe quiser finalizar o programa, digite: exit\n");
}

bool exists_file(const string &name)
{
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

string exec(const char *cmd)
{
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }

    result.pop_back();
    return result;
}

const char protocol[] = "/chat-";
struct mq_attr attr;
mqd_t user_queue;
queue<char *> fila_msg_enviadas;
sem_t S;