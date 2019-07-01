#include "receive_msg.hpp"

void *receive_msg(void *ptr)
{
    char msg_recebida[524], *user, *dest, msg[524];
    int opcao = *((int *)&ptr);

    while (1)
    {
        strcpy(msg, "");
        if ((mq_receive(user_queue, msg_recebida, sizeof(msg_recebida), 0)) < 0)
        {
            perror("mq_receive:");
            exit(1);
        }
        user = strtok(msg_recebida, ":");
        dest = strtok(NULL, ":");
        if (strcmp(dest, "all") == 0)
            strcpy(msg, "Broadcast de ");
        strcat(msg, strtok(NULL, ":"));
        printf("%s:%s> ", user, msg);

        if (opcao == 2)
        {
            string dest_s(dest);
            dest_s.erase(0,1);
            printf("user[%s] dest_s[%s] msg[%s]\nmap[%d] join[%d]\n", user, dest_s.c_str(), msg, users[user], strcmp(msg, "join"));
            if (users[user])
            {
                if (strcmp(msg, "leave") == 0)
                {
                    users.erase(user);
                }
                else
                {
                    for (auto &u : users)
                    {
                        if (u.first != user)
                        {
                            string result = dest_s + ":" + u.first + ":<" + user + ">" + msg;
                            printf("**%s**\n", result.c_str());
                            fila_msg_enviadas.push(result);
                            sem_post(&S);
                        }
                    }
                }
            }
            else
            {
                if (strcmp(msg, "join") == 0)
                {
                    users[user] = true;
                }
                else
                {
                    string result = string(dest_s) + ":" + user + ":NOT A MEMBER.";
                    fila_msg_enviadas.push(result);
                    sem_post(&S);

                }
            }
        }
        fflush(stdout);
    }
}