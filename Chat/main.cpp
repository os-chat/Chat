#include "main_interface.h"
#include "main_console.h"
#include "main_terminal.h"
#include "cmd_list.h"
#include "receive_msg.h"
#include "send_msg.h"

int main(int argc, char *argv[])
{
    sem_init(&S, 0, 0);
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(char) * 524;
    attr.mq_flags = 0;
    char user_name[11];
    strcpy(user_name, argv[1]);
    char nome_negado[] = "all";
    int opcao;

    while (!strcmp(user_name, nome_negado))
    {
        printf("Nao é permitido usar o nome \'%s\', tente novamente com outro nome: ", user_name);
        scanf(" %10[^\n]", user_name);
    }

    string system_user = exec("echo \"$USER\"");
    regex r(R"(([^rwx0-9\-\s]).*?(?=\s))");
    string ls = "ls -l ";
    string pth = "/dev/mqueue/chat-";
    string us(user_name);
    if (exists_file(pth+us)) {
        string output_ls = exec((ls+pth+us).c_str());
        smatch m; 
        regex_search(output_ls, m, r); 
        // enquanto chat-X existir, e nao for pertencente à quem ta tentando criar
        // insere outro nome
        if (m[0] != system_user){
            do 
            {
                printf("Usuário \'%s\' já existe ou é proibido. Tente novamente, com outro nome: ", user_name);
                scanf(" %10[^\n]", user_name);
                if(!strcmp(user_name, nome_negado)) continue;
                us=user_name;
                if(!exists_file(pth+us)) break;
                output_ls = exec((ls+pth+us).c_str());
                regex_search(output_ls, m, r); 
            } while (m[0] != system_user);
        }
    }


    printf("-----------------------");
    for (size_t i = 0; i < strlen(user_name); ++i)
        printf("-");

    printf("\n| Bem-vindo ao Chat, %s |\n", user_name);
    printf("-----------------------");
    for (size_t i = 0; i < strlen(user_name); ++i)
        printf("-");

    printf("\n\nComo deseja usar o Chat?\n");
    printf("1. Terminal\n");
    printf("2. Console\n");
    printf("3. Interface Gráfica\n");
    printf("0. Sair\n");

    printf("\nOpção: ");
    scanf(" %d", &opcao);

    while (opcao < 0 || opcao > 3)
    {
        printf("Opção deve ser 0, 1, 2 ou 3, tente novamente: ");
        scanf(" %d", &opcao);
    }

    system("clear");

    if (opcao == 1)
        main_terminal(user_name);
    else if (opcao == 2)
    {
        main_console(user_name);
    }
    else if (opcao == 3)
    {
        main_interface(argc, argv, user_name);
    }
    else
        printf("Encerrando\n");

    return 0;
}

//   0   5s    1    10s    2   15s    3
