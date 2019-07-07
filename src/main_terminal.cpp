#include "main_terminal.hpp"

void main_terminal(const string user_name) {
    signal(SIGINT, handle_sigint);
    string user_queue_name(protocol);
    user_queue_name += user_name;
    mode_t prev_umask = umask(0155);
    if ((user_queue = mq_open(user_queue_name.c_str(), O_RDWR | O_CREAT, 0622, &attr)) < 0) {
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

        scanf(" %10[^:\n]:%10[^:\n]:%500[^\n]", user_c, destinatario_c, texto_c);

        if(destinatario_c[0] == '#') {
            grupo(user_c, destinatario_c, texto_c);
            continue;
        }
            

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

        string destinatario(destinatario_c);

        if(user == "cc") {
            string channel_name = "/canal-" + destinatario;
            mode_t prev_umask = umask(0155);
            mqd_t channel;
            if ((channel = mq_open(channel_name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0622, &attr)) < 0 && errno == EEXIST) {
                perror("mq_open");
                umask(prev_umask);
                printf("Erro ao criar canal \'%s\'\n", destinatario_c);
            }
            umask(prev_umask);
            vector<string> user;
            user.push_back(user_atual);
            canais.push_back({ .dono = user_atual, .mq_canal = channel, .nome = destinatario, .usuarios = user });

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

        string texto(texto_c), msg_enviada;

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

void grupo(const char u[], const char d[], const char m[]) {
    string user(u), dest(d), msg(m);
    dest.erase(0, 1);
    canal *channel;
    bool find_channel = false;

    for (auto c : canais) {
        if (c.nome == dest) {
            channel = &c;
            find_channel = true;
            break;
        }
    }

    for(auto u : channel->usuarios) {
        printf("U = (%s)\n", u.c_str());
    }

    if(!find_channel) {
        printf("Canal não encontrado.\n");
        return;
    }

    if(msg == "join") {
        channel->usuarios.push_back(user);
        printf("Adicionado ao canal \'%s\'.\n", dest.c_str());

        return;
    }

    auto it = find(channel->usuarios.begin(), channel->usuarios.end(), user_atual);
    if(it == channel->usuarios.end()) {
        printf("#%s:%s:NOT A MEMBER\n", dest.c_str(), user_atual.c_str());
    }

    if(msg == "leave") {
        channel->usuarios.erase(it);
        printf("Removido com sucesso do canal\n");
        return;
    }

    if(msg == "destroy") {
        if(channel->dono == user_atual) {

            mq_close(channel->mq_canal);
            mq_unlink(channel->nome.c_str());
        }
        else {
            printf("Você não é o dono do canal.\n");
        }

        return;
    }

    string mensagem = user_atual + ":#" + dest + ":" + msg;
    fila_msg_enviadas.push(mensagem);

    sem_post(&S);
}