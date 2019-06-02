#include "send_msg.h"
// send msg to one unique user
void *unique_send(void *ptr)
{
    mqd_t other_queue;
    string protc = protocol;
    string user = (char *)ptr;
    string res = protc+user;
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
            sleep(5 * tentativas);
        }
        else
            break;
    }
    if (tentativas > 3)
        printf("ERRO %s", msg_enviada);

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
        strcpy(msg_enviada, fila_msg_enviadas.front());

        user_name = strtok(token, ":");
        destinatario = strtok(NULL, ":");

        if (strcmp(destinatario, "all") == 0)
        { // se o destinatário for all
            vector<char *> users = cmd_list();
            for (auto u : users)
            {
                if (strcmp(user_name, u))
                {
                    strcpy(other_queue_name, protocol);
                    strcat(other_queue_name, u);
                    other_queue = mq_open(other_queue_name, O_WRONLY | O_NONBLOCK);
                    if (mq_send(other_queue, msg_enviada, sizeof(msg_enviada), 0) < 0)
                    {
                        perror("mq_send");
                        exit(1);
                    }

                    mq_close(other_queue);
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
                int tentativas = 0;
                while (tentativas <= 3)
                {
                    if (mq_send(other_queue, msg_enviada, sizeof(msg_enviada), 0) < 0 && errno == EAGAIN)
                    {
                        tentativas++;
                        if (tentativas == 4)
                            break;
                        sleep(5 * tentativas);
                    }
                    else
                        break;
                }
                if (tentativas > 3)
                    printf("ERRO %s", msg_enviada);

                mq_close(other_queue);
            }
        }
    }
}