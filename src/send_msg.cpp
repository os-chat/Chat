#include "send_msg.hpp"

string user_name;
string destinatario;
string texto;

// send msg to one unique user
void *unique_send(void *ptr) {
    mqd_t other_queue;
    string protc = protocol;
    string user = (char *)ptr;
    string res = protc + user;
    other_queue = mq_open(res.c_str(), O_WRONLY | O_NONBLOCK);
    string msg_enviada = fila_msg_enviadas.front();
    msg_enviada+=":";
    msg_enviada+=to_string(generate_key())+"\n";

    int tentativas = 0;
    while (tentativas <= 3) {
        if (mq_send(other_queue, msg_enviada.c_str(), sizeof(msg_enviada), 0) < 0 && errno == EAGAIN) {
            tentativas++;
            if (tentativas == 4)
                break;
            sleep(1 + tentativas);
        }
        else {
            break;
        }
    }
    if (tentativas > 3) {
        printf("ERRO %s\n> ", msg_enviada.c_str());
    }

    mq_close(other_queue);
    pthread_exit(NULL);
}

void *send_canal(void *ptr) {
    string channel_name = "/canal-" + user_name;
    mqd_t channel_queue = mq_open(channel_name.c_str(), O_WRONLY | O_NONBLOCK);
    string mensagem = destinatario; // user
    if(destinatario != "destroy")
        mensagem += ":" + texto; // user:texto
    
    mq_send(channel_queue, mensagem.c_str(), sizeof(mensagem), 0);
    mq_close(channel_queue);
    pthread_exit(NULL);
}

void *send_msg(void *ptr) {
    while (1) {
        sem_wait(&S);

        mqd_t other_queue;
        string other_queue_name;
        string token= fila_msg_enviadas.front();
        auto tokens = split(token.c_str());
        user_name = tokens[0];
        destinatario = tokens[1];

        if(destinatario == "destroy") {
            user_name.erase(0,1);
            pthread_t t;
            pthread_create(&t, NULL, send_canal, NULL);
            pthread_join(t, NULL);
        }

        texto = tokens[2];
        vector<string> user_list = cmd_list();

        if (destinatario == "all") { // se o destinatário for all
            vector<pthread_t> thread;
            for (size_t i = 0; i < user_list.size(); ++i) {
                pthread_t t;
                thread.push_back(t);
            }

            for (size_t i = 0; i < thread.size(); ++i) {
                if (user_name != user_list[i]) {
                    pthread_create(&thread[i], NULL, unique_send, (void *)user_list[i].c_str());
                }
            }

            for(size_t i = 0; i < thread.size(); ++i) {
                pthread_join(thread[i], NULL);
            }
        }
        else if(user_name[0] == '#') { // Se for #canal:...
            user_name.erase(0,1);
            pthread_t t;
            pthread_create(&t, NULL, send_canal, NULL);
            pthread_join(t, NULL);
        }
        else {
            other_queue_name=protocol+destinatario;

            // O_WRONLY = Open - Write Only
            if ((other_queue = mq_open(other_queue_name.c_str(), O_WRONLY | O_NONBLOCK)) < 0) {
                printf("UNKNOWNUSER %s\n> ", destinatario.c_str());
                fflush(stdout);
            }
            else {
                pthread_t t;
                pthread_create(&t, NULL, unique_send, (void *)destinatario.c_str());
                pthread_join(t, NULL);
            }
        }
        fila_msg_enviadas.pop();
    }
}