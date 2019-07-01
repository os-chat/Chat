#include "main_terminal.hpp"

void individual(string user_name);
void group(string user_name);

void main_terminal(const string user_name, int opcao)
{
    signal(SIGINT, handle_sigint);
    string protocol = ((opcao == 1) ? (user_protocol) : (group_protocol));
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
    pthread_create(&thread_recebe, NULL, &receive_msg, (void*)opcao);
    pthread_create(&thread_envia, NULL, &send_msg, NULL);

    if(opcao == 1) individual(user_name);
    else group(user_name);

    mq_close(user_queue);
    mq_unlink(user_queue_name.c_str());
}

void individual(string user_name) {
    while (1) {
        char user_c[11], destinatario_c[11], texto_c[501];
        strcpy(user_c, "");
        strcpy(destinatario_c, "");
        strcpy(texto_c, "");
        printf("> ");

        scanf(" %10[^:\n]:%10[^:]:%500[^\n]", user_c, destinatario_c, texto_c);

        string user(user_c);

        if (user == "exit" || user == "sair")
        {
            float delay = 0.5*joined_groups.size();
            for (auto u : joined_groups){
                string leave=user_name+":#"+u.first+":leave";
                cout << leave << endl;
                fila_msg_enviadas.push(leave);
                sem_post(&S);
            }
            // break;
            sleep(delay);
            break;
        }

        if (user == "list")
        {
            printf("\n\nO que deseja listar?\n");
            printf("1. Usuários\n");
            printf("2. Salas de bate-papo\n");
            printf("0. Voltar\n");

            printf("\nOpção: ");
            int opcao;
            scanf(" %d", &opcao);

            while (opcao < 0 || opcao > 2)
            {
                printf("Opção deve ser 0, 1 ou 2. Tente novamente: ");
                scanf(" %d", &opcao);
            }

            system("clear");

            if (opcao == 1)
            {
                printf("\nLista de Usuários:\n");
                vector<string> cmd_users = cmd_list("chat");
                for (size_t i = 0; i < cmd_users.size(); ++i)
                {
                    printf("%ld - %s\n", i + 1, cmd_users[i].c_str());
                }
            }
            if (opcao == 2)
            {
                printf("\nLista de Salas de Bate-Papo:\n");
                vector<string> cmd_users = cmd_list("canal");
                for (size_t i = 0; i < cmd_users.size(); ++i)
                {
                    printf("%ld - %s\n", i + 1, cmd_users[i].c_str());
                }  
            }    

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

        msg_enviada = user;
        msg_enviada += ":";
        msg_enviada += destinatario;
        msg_enviada += ":";
        msg_enviada += texto;
        msg_enviada += ":";
        msg_enviada += to_string(generate_key()) + "\n";
        fila_msg_enviadas.push(msg_enviada);
        
        if (destinatario[0]=='#'&&texto=="leave") {
            destinatario.erase(0,1);
            joined_groups.erase(destinatario);
        }
        sem_post(&S);
    }
}

void group(string user_name)
{
    while (1)
    {
        string msg;
        getline(cin, msg);
        if (msg == "destroy")
        {
            fila_msg_enviadas.push("#" + user_name + ": Destroyed.");
            sem_post(&S);
            break;
        }
        if (msg == "exit" || msg == "sair")
        {
            break;
        }
        if (msg == "list") {
            printf("\nLista de Usuários:\n");
            int i =1;
            for(auto& u : users) {
                cout << i++ << ". " << u.first << endl;
            }
            printf("\n");
            continue;
        }

    }
}