#include "send_msg.h"
typedef struct __opt_msg
{
    int opt;
    char msg[501];
} opt_msg;
// send msg to one unique user
void *unique_send(void *ptr)
{
    mqd_t other_queue;
    string protc = protocol;
    opt_msg control = *(opt_msg *)ptr;
    string user = control.msg;
    string res = protc + user;
    other_queue = mq_open(res.c_str(), O_WRONLY | O_NONBLOCK);
    string msg_enviada = fila_msg_enviadas.front();

    int tentativas = 0;
    while (tentativas <= 3)
    {
        if (mq_send(other_queue, msg_enviada.c_str(), sizeof(msg_enviada), 0) < 0 && errno == EAGAIN)
        {
            tentativas++;
            if (tentativas == 4)
                break;
            sleep(1 + tentativas);
        }
        else
        {
            break;
        }
    }
    if (tentativas > 3)
    {
        if (control.opt == 1)
        {
            printf("ERRO %s\n> ", msg_enviada.c_str());
            fflush(stdout);
        }
        if (control.opt == 2)
        {
            printw("ERRO %s\n> ", msg_enviada.c_str());
            refresh();
        }
    }

    mq_close(other_queue);
    pthread_exit(NULL);
}

void *send_msg(void *ptr)
{
    int opt = *((int *)(&ptr));

    while (1)
    {
        sem_wait(&S);

        mqd_t other_queue;
        string other_queue_name;
        string user_name, destinatario;
        string token= fila_msg_enviadas.front();
        auto tokens = split(token.c_str());
        user_name = tokens[0];
        destinatario = tokens[1];
        vector<string> user_list = cmd_list();

        opt_msg control;
        control.opt = opt;

        if (destinatario == "all")
        { // se o destinatário for all
            vector<pthread_t> thread;
            for (size_t i = 0; i < user_list.size(); ++i)
            {
                pthread_t t;
                thread.push_back(t);
            }

            for (size_t i = 0; i < thread.size(); ++i)
            {
                if (user_name != user_list[i])
                {
                    strcpy(control.msg, user_list[i].c_str());
                    pthread_create(&thread[i], NULL, unique_send, &control);
                }
            }
        }
        else
        {
            other_queue_name=protocol+destinatario;

            // O_WRONLY = Open - Write Only
            if ((other_queue = mq_open(other_queue_name.c_str(), O_WRONLY | O_NONBLOCK)) < 0)
            {
                if (opt == 1)
                {
                    printf("UNKNOWNUSER %s\n> ", destinatario.c_str());
                }
                if (opt == 2)
                {
                    printw("UNKNOWNUSER %s\n> ", destinatario.c_str());
                    refresh();
                }
                fflush(stdout);
            }
            else
            {
                pthread_t t;
                strcpy(control.msg, destinatario.c_str());
                pthread_create(&t, NULL, unique_send, &control);
            }
        }
    }
}
