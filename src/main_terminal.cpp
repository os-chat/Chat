#include "main_terminal.hpp"

void *monitora_canal(void *ptr) {
    string channel = (char *)ptr;
    string channel_name = "/canal-" + channel;
    mode_t prev_umask = umask(0155);
    mqd_t channel_queue;
    if ((channel_queue = mq_open(channel_name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0622, &attr)) < 0 && errno == EEXIST) {
        umask(prev_umask);
        printf("Erro ao criar canal \'%s\'\n", channel.c_str());
        pthread_exit(NULL);
    }
    umask(prev_umask);
    char msg_recebida[524];
    vector<string> users;
    users.push_back(user_atual);
    while(1) {
        if(mq_receive(channel_queue, msg_recebida, sizeof(msg_recebida), 0) < 0) {
            perror("mq_receive:");
            exit(1);
        }

        if(strcmp(msg_recebida, "destroy") == 0) {
            string mensagem = "#" + channel + ":destroyed";
            for (auto u : users) {
                string queue_name = "/chat-" + u;
                mqd_t user_queue;

                if((user_queue = mq_open(queue_name.c_str(), O_WRONLY | O_NONBLOCK)) < 0) {
                    continue;
                }
                
                if(mq_send(user_queue, mensagem.c_str(), sizeof(mensagem), 0) < 0) {
                    continue;
                }
            }

            mq_close(channel_queue);
            mq_unlink(channel_name.c_str());
            break;
        }

        char *user = strtok(msg_recebida, ":");
        char *msg = strtok(NULL, ":");

        if(strcmp(msg, "join") == 0) {
            string u(user);
            users.push_back(u);
            continue;
        }

        if(strcmp(msg, "leave") == 0) {
            string u(user);
            auto it = find(users.begin(), users.end(), u);
            if(it != users.end()) users.erase(it);
            continue;
        }

        string user_str(user);

        if(find(users.begin(), users.end(), user_str) != users.end()) { // Usuário é membro
            for(auto u : users) { // Para cada usuário
                if(u != user_str) { // Enviar para todos, menos para si mesmo
                    string mensagem = "#" + channel + ":" + u + ":<" + user_str + ">" + msg; // #canal:user:<expedidor>texto
                    string queue_name = "/chat-" + u;
                    mqd_t user_queue = mq_open(queue_name.c_str(), O_WRONLY | O_NONBLOCK);
                    mq_send(user_queue, mensagem.c_str(), sizeof(mensagem), 0);
                }
            }
        }
        else { // Usuário não é membro
            string mensagem = "#" + channel + ":" + user_str + ":NOT A MEMBER"; // #canal:user:NOT A MEMBER
            string queue_name = "/chat-" + user_str;
            mqd_t user_queue = mq_open(queue_name.c_str(), O_WRONLY | O_NONBLOCK);
            mq_send(user_queue, mensagem.c_str(), sizeof(mensagem), 0);
        }
    }

    pthread_exit(NULL);
}

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
        fflush(stdout);
        scanf(" %10[^:\n]:%10[^:\n]:%500[^\n]", user_c, destinatario_c, texto_c);

        if(destinatario_c[0] == '#' or user_c[0] == '#') {
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
            canais.push_back(destinatario);
            pthread_t t;
            pthread_create(&t, NULL, monitora_canal, (void *)destinatario.c_str());
            continue;
        }

/*         if (user != user_name) {
            printf("Expedidor inválido, tente novamente.\n");
            continue;
        } */

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

    while(canais.size()) {
        string canal = "#" + canais[0];
        grupo(canal.c_str(), "destroy", ""); // Destruir canal
        sleep(1);
    }

    mq_close(user_queue);
    mq_unlink(user_queue_name.c_str());
}

void grupo(const char u[], const char d[], const char m[]) {
    string user(u), dest(d), msg(m);
    string mensagem = dest + ":" + user + ":" + msg;
    bool encontrado = false;
    if(dest == "destroy" && user[0] == '#' && msg == "") {
        user.erase(0,1);
        auto it = find(canais.begin(), canais.end(), user);
        if(it != canais.end()) { // Se encontrar canal na lista de canais que é dono                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
            string mensagem = "#" + user + ":" + dest; // #canal:destroy
            fila_msg_enviadas.push(mensagem);
            sem_post(&S);
            canais.erase(it);
        }
        else // Se não encontrar, não é dono
            printf("Você não é o dono do canal\n");

        return;
    }
    else {
        dest.erase(0,1);
        DIR *d;
        struct dirent *dir;
        d = opendir("/dev/mqueue");
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                char *token;
                token = strtok(dir->d_name, "-");
                if (strcmp(token, "canal") == 0) {
                    token = strtok(NULL, "-");
                    if(string(token) == dest) {
                        encontrado = true;
                        break;
                    }
                }
            }

            closedir(d);

            if(!encontrado) { // Se não encontrar na /dev/mqueue
                printf("Canal não encontrado\n");
                return;
            }
        }

        fila_msg_enviadas.push(mensagem); // #canal:user:texto
        sem_post(&S);
        return;
    }
}