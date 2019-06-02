#include "main_console.h"

bool iequals(const string &a, const string &b)
{
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}
void main_console(char *user_name, int opcao)
{
    int ch;

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

    WINDOW *win = initscr();
    raw();
    keypad(stdscr, TRUE);
    scrollok(stdscr,TRUE);
    printw("> ");
    vector<string> history;
    string x = "", last;
    bool exit = false;
    int row = 0, col = 0, it = 0, up_down = 0;
    while (exit == false)
    {
        ch = getch();
        switch (ch)
        {
        case KEY_UP:
            if (!up_down) {
                last = x;
            }
            if (it - (up_down + 1) >= 0)
            {
                getyx(win, row, col);
                move(row, 0); // move to begining of line
                clrtoeol();   // clear line
                x = history[it - (++up_down)];
                addstr(("> " + x).c_str());
            }
            break;
        case KEY_DOWN:
            if (up_down)
            {
                getyx(win, row, col);
                move(row, 0); // move to begining of line
                clrtoeol();   // clear line
                --up_down;
                if (up_down)
                {
                    x = history[it - up_down];
                    addstr(("> " + x).c_str());
                }
                else
                {
                    x = last;
                    addstr(("> " + x).c_str());
                }
            }
            break;
        case KEY_LEFT:
            getyx(win, row, col);
            if (col > 2)
            {
                move(row, col - 1);
            }
            break;
        case KEY_RIGHT:
            getyx(win, row, col);
            if (col <= x.size()+1)
                move(row, col + 1);
            break;
        case KEY_BACKSPACE:
            if (x.size())
            {
                x.pop_back();
                delch();
            }
            else
            {
                // impede que apague o "> "
                getyx(win, row, col);
                move(row, 2);
            }
            break;
        case 10:
            if (iequals(x, "exit"))
            {
                exit = true;
                break;
            }
            printw("> %s\n> ", x.c_str());
            if (x.size())
            {
                history.push_back(x);
                it++;
            }
            char user[11], destinatario[11], texto[501];
            char msg_enviada[524];
            strcpy(user, "");
            strcpy(destinatario, "");
            strcpy(texto, "");

            sscanf(x.c_str(), " %10[^:\n]:%10[^:]:%500[^\n]", user, destinatario, texto);
            x = "";
            up_down = 0;

            if (!strcmp(user, "list"))
            {
                printw("Lista de Usuários:\n");
                vector<char *> users = cmd_list();
                for (size_t i = 0; i < users.size(); ++i)
                {
                    printw("%d - %s\n", i + 1, users[i]);
                }
                printw("\n> ");
                break;
            }

            if (!strlen(destinatario) || !strlen(texto) || !strlen(user))
            {
                printw("Formato inválido, tente novamente.\n> ");
                break;
            }

            if (strcmp(user, user_name))
            {
                printw("Expedidor inválido, tente novamente.\n> ");
                break;
            }

            strcpy(msg_enviada, user);
            strcat(msg_enviada, ":");
            strcat(msg_enviada, destinatario);
            strcat(msg_enviada, ":");
            strcat(msg_enviada, texto);
            strcat(msg_enviada, "\n");

            fila_msg_enviadas.push(msg_enviada);
            sem_post(&S);

            break;

        default:
            x += ch;
            col++;
            break;
        }
    }

    endwin();
    mq_close(user_queue);
    mq_unlink(user_queue_name);
}
