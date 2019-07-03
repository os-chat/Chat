#include "main_terminal.hpp"
#include "cmd_list.hpp"
#include "receive_msg.hpp"
#include "send_msg.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Modo de utilização: ./Chat <nome-do-usuário>\nEncerrando programa.\n");
        return 0;
    }
    sem_init(&S, 0, 0);
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(char) * 524;
    attr.mq_flags = 0;
    string user_name(argv[1]);
    const vector<string> nome_negado = {"all", "exit"};

    while(find(nome_negado.begin(), nome_negado.end(), user_name) != nome_negado.end()) {
        printf("Nao é permitido usar o nome \'%s\', tente novamente com outro nome: ", user_name.c_str());
        char novo_nome[11];
        scanf(" %10[^\n]", novo_nome);
        user_name = novo_nome;
    }

    string system_user = exec("echo \"$USER\"");
    regex r(R"(([^rwx0-9\-\s]).*?(?=\s))");
    string ls = "ls -l ";
    string pth = "/dev/mqueue/chat-";
    string us(user_name);
    if (exists_file(pth + us))
    {
        string output_ls = exec((ls + pth + us).c_str());
        smatch m;
        regex_search(output_ls, m, r);
        // enquanto chat-X existir, e nao for pertencente à quem ta tentando criar
        // insere outro nome
        if (m[0] != system_user)
        {
            do
            {
                printf("Usuário \'%s\' já existe ou é proibido. Tente novamente, com outro nome: ", user_name.c_str());
                char novo_nome[11];
                scanf(" %10[^\n]", novo_nome);
                user_name = novo_nome;
                if (find(nome_negado.begin(), nome_negado.end(), user_name) != nome_negado.end())
                    continue;
                us = user_name.c_str();
                if (!exists_file(pth + us))
                    break;
                output_ls = exec((ls + pth + us).c_str());
                regex_search(output_ls, m, r);
            } while (m[0] != system_user);
        }
    }

    printf("-----------------------");
    for (size_t i = 0; i < user_name.size(); ++i)
        printf("-");

    printf("\n| Bem-vindo ao Chat, %s |\n", user_name.c_str());
    printf("-----------------------");
    for (size_t i = 0; i < user_name.size(); ++i)
        printf("-");
    
    printf("\n");

    user_atual = user_name;
    main_terminal(user_name);

    return 0;
}