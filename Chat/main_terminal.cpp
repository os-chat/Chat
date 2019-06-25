#include "main_terminal.h"

void main_terminal(char *user_name, int opcao)
{
    map<int,bool> keys;
    signal(SIGINT, handle_sigint);
    char user_queue_name[20];
    strcpy(user_queue_name, protocol);
    strcat(user_queue_name, user_name);
    mode_t prev_umask = umask(0155);
    if ((user_queue = mq_open(user_queue_name, O_RDWR | O_CREAT, 0622, &attr)) < 0)
    {
        perror("mq_open");
        umask(prev_umask);
        exit(1);
    }
    umask(prev_umask);
    pthread_t thread_recebe, thread_envia;
    pthread_create(&thread_recebe, NULL, &receive_msg, (void*)opcao);
    pthread_create(&thread_envia, NULL, &send_msg, (void *)opcao);

    while (1)
    {
        char user[11], destinatario[11], texto[501];
        string msg_enviada;
        strcpy(user, "");
        strcpy(destinatario, "");
        strcpy(texto, "");
        printf("> ");

        scanf(" %10[^:\n]:%10[^:]:%500[^\n]", user, destinatario, texto);

        if (!strcmp(user, "exit") || !strcmp(user, "sair"))
        {
            break;
        }

        if (!strcmp(user, "list"))
        {
            printf("\nLista de Usuários:\n");
            vector<string> users = cmd_list();
            for (size_t i = 0; i < users.size(); ++i)
                printf("%d - %s\n", i + 1, users[i].c_str());
            printf("\n");
            continue;
        }

        if (!strlen(destinatario) || !strlen(texto) || !strlen(user))
        {
            printf("Formato inválido, tente novamente.\n");
            continue;
        }

        if (strcmp(user, user_name))
        {
            printf("Expedidor inválido, tente novamente.\n");
            continue;
        }
            msg_enviada=user;
            msg_enviada+=":";
            msg_enviada+=destinatario;
            msg_enviada+=":";
            msg_enviada+=texto;
            msg_enviada+=":";
            msg_enviada+=to_string(generate_key(keys))+"\n";

            fila_msg_enviadas.push(msg_enviada);
        sem_post(&S);
    }

    mq_close(user_queue);
    mq_unlink(user_queue_name);
}
