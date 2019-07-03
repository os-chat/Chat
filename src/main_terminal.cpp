#include "main_terminal.hpp"

void main_terminal(const string user_name) {
    signal(SIGINT, handle_sigint);
    string user_queue_name(protocol);
    user_queue_name += user_name;
    mode_t prev_umask = umask(0155);
    if ((user_queue = mq_open(user_queue_name.c_str(), O_RDWR | O_CREAT, 0622, &attr)) < 0)
    {
        perror("mq_open");
        umask(prev_umask);
        exit(1);
    }
    umask(prev_umask);
    pthread_t thread_recebe, thread_envia;
    pthread_create(&thread_recebe, NULL, &receive_msg, NULL);
    pthread_create(&thread_envia, NULL, &send_msg, NULL);

    while (1) {
        char user_c[11], destinatario_c[11], texto_c[501];
        strcpy(user_c, "");
        strcpy(destinatario_c, "");
        strcpy(texto_c, "");
        printf("> ");

        scanf(" %10[^:\n]:%10[^:]:%500[^\n]", user_c, destinatario_c, texto_c);

        string user(user_c);

        if (user == "exit" || user == "sair") {
            break;
        }

        if (user == "list") {
            printf("\nLista de Usuários:\n");
            vector<string> users = cmd_list();
            for (size_t i = 0; i < users.size(); ++i)
                printf("%ld - %s\n", i + 1, users[i].c_str());
            printf("\n");
            continue;
        }

        if (user != user_name) {
            printf("Expedidor inválido, tente novamente.\n");
            continue;
        }

        if (!strlen(destinatario_c) || !strlen(texto_c)) {
            printf("Formato inválido, tente novamente.\n");
            continue;
        }

        string destinatario(destinatario_c), texto(texto_c), msg_enviada;

        msg_enviada=user;
        msg_enviada+=":";
        msg_enviada+=destinatario;
        msg_enviada+=":";
        msg_enviada+=texto;
        fila_msg_enviadas.push(msg_enviada);

        sem_post(&S);
    }

    mq_close(user_queue);
    mq_unlink(user_queue_name.c_str());
}