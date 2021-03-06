#include "common.hpp"

int generate_key() {
    srand(time(NULL));
    int key;
    do {
       key = rand();
    } while (keys.find(key) != keys.end());
    keys[key]=true;
    return key;
}

bool check_key(int key) {
    return keys[key];
}

void handle_sigint(int sig) {
    printf("\nSe quiser finalizar o programa, digite: exit\n");
    fflush(stdout);
}

bool exists_file(const string &name) {
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

vector<string> split(const char *str, char c/*=':'*/) {
    vector<string> result;

    do {
        const char *begin = str;

        while(*str != c && *str)
            str++;

        result.push_back(string(begin, str));
    } while (0 != *str++);

    return result;
}

const char protocol[] = "/chat-";
struct mq_attr attr;
mqd_t user_queue;
queue<string> fila_msg_enviadas;
sem_t S;
map<int,bool> keys;
string user_atual;
vector<string> canais;