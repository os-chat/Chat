#include "send_msg.h"

// send msg to one unique user
void *unique_send(void *ptr)
{
    mqd_t other_queue;
    string protc = protocol;
    string user = (char *)ptr;
    string res = protc + user;
    other_queue = mq_open(res.c_str(), O_WRONLY | O_NONBLOCK);
    char msg_enviada[501];
    strcpy(msg_enviada, fila_msg_enviadas.front());

    int tentativas = 0;
    while (tentativas <= 3)
    {
        if (mq_send(other_queue, msg_enviada, sizeof(msg_enviada), 0) < 0 && errno == EAGAIN)
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
        printf("ERRO %s", msg_enviada);
    }

    mq_close(other_queue);
    pthread_exit(NULL);
}

void *send_msg(void *ptr)
{
    while (1)
    {
        sem_wait(&S);

        mqd_t other_queue;
        char other_queue_name[20];
        char *user_name, *destinatario, msg_enviada[524], token[524];
        strcpy(token, fila_msg_enviadas.front());
        user_name = strtok(token, ":");
        destinatario = strtok(NULL, ":");
        vector<string> user_list = cmd_list();

        if (strcmp(destinatario, "all") == 0)
        { // se o destinatário for all
            vector<pthread_t> thread;
            for (size_t i = 0; i < user_list.size(); ++i)
            {
                pthread_t t;
                thread.push_back(t);
            }

            for (size_t i = 0; i < thread.size(); ++i)
            {
                if (strcmp(user_name, user_list[i].c_str()))
                {
                    pthread_create(&thread[i], NULL, unique_send, (void *)user_list[i].c_str());
                }
            }
        }
        else
        {
            strcpy(other_queue_name, protocol);
            strcat(other_queue_name, destinatario);

            // O_WRONLY = Open - Write Only
            if ((other_queue = mq_open(other_queue_name, O_WRONLY | O_NONBLOCK)) < 0)
            {
                printf("UNKNOWNUSER %s\n", destinatario);
            }
            else
            {
                pthread_t t;
                pthread_create(&t, NULL, unique_send, (void *)destinatario);
            }
        }
    }
}
