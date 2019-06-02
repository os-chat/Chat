#include "main_terminal.h"

void main_terminal(char *user_name, int opcao)
{
    signal(SIGINT, handle_sigint);
    char user_queue_name[20];
    strcpy(user_queue_name, protocol);
    strcat(user_queue_name, user_name);
    string current_umask = "umask " + exec("umask");
    system("umask u=rw,g=w,o=w");
    if ((user_queue = mq_open(user_queue_name, O_RDWR | O_CREAT, 0622, &attr)) < 0)
    {
        perror("mq_open");
        system(current_umask.c_str());
        exit(1);
    }
    system(current_umask.c_str());

    pthread_t thread_recebe, thread_envia;
    pthread_create(&thread_recebe, NULL, &receive_msg, (void*)opcao);
    pthread_create(&thread_envia, NULL, &send_msg, NULL);

    while (1)
    {
        char user[11], destinatario[11], texto[501];
        char msg_enviada[524];
        strcpy(user, "");
        strcpy(destinatario, "");
        strcpy(texto, "");
        printf("> ");

        scanf(" %10[^:\n]:%10[^:]:%500[^\n]", user, destinatario, texto);

        if (!strcmp(user, "exit"))
        {
            break;
        }

        if (!strcmp(user, "list"))
        {
            printf("\nLista de Usuários:\n");
            vector<char *> users = cmd_list();
            for (int i = 0; i < users.size(); ++i)
                printf("%d - %s\n", i + 1, users[i]);
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

        strcpy(msg_enviada, user);
        strcat(msg_enviada, ":");
        strcat(msg_enviada, destinatario);
        strcat(msg_enviada, ":");
        strcat(msg_enviada, texto);
        strcat(msg_enviada, "\n");

        fila_msg_enviadas.push(msg_enviada);
        sem_post(&S);
    }

    mq_close(user_queue);
    mq_unlink(user_queue_name);
}