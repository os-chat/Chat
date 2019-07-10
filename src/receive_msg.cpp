#include "receive_msg.hpp"

map<int, string> mensagens;

void *receive_msg(void *ptr) {
    char msg_recebida[524], *user, *dest, msg[524], autenticacao[15];
    int cod;
    mqd_t other_queue;

    while (1) {
        strcpy(msg, "");
        if ((mq_receive(user_queue, msg_recebida, sizeof(msg_recebida), 0)) < 0) {
            perror("mq_receive:");
            exit(1);
        }

        user = strtok(msg_recebida, ":");
        dest = strtok(NULL, ":");

        if(user[0] == '#') { // #canal:...
            string u(user), d(dest);
            string mensagem = u + ":" + d;

            if(d != "destroyed") { // #canal:destroyed
                strcpy(msg, strtok(NULL, ":"));
                string m(msg);
                mensagem += ":" + m; // #canal:user:texto
            }

            printf("%s\n> ", mensagem.c_str());
            continue;
        }

        if (strcmp(dest, "all") == 0)
            strcpy(msg, "Broadcast de ");

        strcat(msg, user);
        strcat(msg, ": ");
        strcpy(autenticacao, strtok(NULL, ":"));
        strcat(msg, autenticacao);

        string user_name = user;
        string dest_name = dest;
        string protc = protocol;

        if(autenticacao[0] == '|' && autenticacao[strlen(autenticacao)-1] == '?') {
            int cod = stoi(strtok(autenticacao, "|"));
            string msg = dest_name + ":" + user_name + ":";
            if(check_key(cod)) {
                msg += "|" + to_string(cod) + "Y|";
                keys.erase(cod);
            }
            else {
                msg += "|" + to_string(cod) + "N|";
            }
            
            string queue_name = protc + user_name;
            other_queue = mq_open(queue_name.c_str(), O_WRONLY | O_NONBLOCK);
            mq_send(other_queue, msg.c_str(), sizeof(msg), 0);
        }
        else if(autenticacao[0] == '|' && (autenticacao[strlen(autenticacao)-2] == 'Y' || autenticacao[strlen(autenticacao)-2] == 'N')) {
            char ok = autenticacao[strlen(autenticacao)-2];
            int cod = stoi(strtok(autenticacao, "|"));
            if(ok == 'Y') {
                printf("%s\n> ", mensagens[cod].c_str());
                mensagens.erase(cod);
            }
        }
        else {
            cod = stoi(strtok(NULL, ":"));
            string queue_name = protc + user_name;
            other_queue = mq_open(queue_name.c_str(), O_WRONLY | O_NONBLOCK);

            string cod_str = user_atual + ":" + user_name + ":|" + to_string(cod) + "|?";

            mensagens[cod] = msg;
            mq_send(other_queue, cod_str.c_str(), sizeof(cod_str), 0);
        }
    }
}